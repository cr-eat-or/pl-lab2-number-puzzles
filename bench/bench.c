#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puzzle.h"
#include "solver.h"

#ifdef _WIN32
#include <windows.h>

/* Текущее время в миллисекундах. */
static double now_ms(void)
{
    LARGE_INTEGER freq, cnt;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart * 1000.0 / (double)freq.QuadPart;
}
#else
#include <time.h>

static double now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
#endif

/* Замеряет один ребус: repeats запусков, считает среднее и минимум в мс.
   Возвращает 0, если на всех запусках найдено решение. */
static int bench_puzzle(const char *line, int repeats, double *avg, double *min)
{
    char answer[256];
    double total = 0.0;

    *min = 0.0;
    for (int i = 0; i < repeats; i++) {
        double start = now_ms();
        int r = solve_puzzle(line, answer, sizeof(answer));
        double t = now_ms() - start;

        if (r != 0)
            return -1;
        total += t;
        if (i == 0 || t < *min)
            *min = t;
    }
    *avg = total / repeats;
    return 0;
}

int main(int argc, char **argv)
{
    const char *path;
    int repeats = 3;
    FILE *f;
    char line[256];
    int failed = 0;

    if (argc < 2) {
        fprintf(stderr, "Использование: bench <файл с ребусами> [повторов, по умолчанию 3]\n");
        return 2;
    }
    path = argv[1];
    if (argc > 2)
        repeats = atoi(argv[2]);
    if (repeats < 1) {
        fprintf(stderr, "Число повторов должно быть положительным\n");
        return 2;
    }

    f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Не удалось открыть %s\n", path);
        return 2;
    }

    /* CSV в stdout, ход выполнения в stderr */
    printf("puzzle,letters,words,repeats,avg_ms,min_ms\n");
    while (fgets(line, sizeof(line), f)) {
        Puzzle p;
        double avg, min;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        if (puzzle_parse(line, &p) != 0) {
            fprintf(stderr, "Ошибка разбора: %s\n", line);
            failed++;
            continue;
        }
        if (bench_puzzle(line, repeats, &avg, &min) != 0) {
            fprintf(stderr, "Нет решения: %s\n", line);
            failed++;
            continue;
        }
        printf("%s,%d,%d,%d,%.3f,%.3f\n", line, p.letter_count, p.word_count, repeats, avg, min);
        fflush(stdout);
        fprintf(stderr, "%-60s %2d букв  %10.3f мс\n", line, p.letter_count, avg);
    }
    fclose(f);
    return failed ? 1 : 0;
}
