#include <stdio.h>
#include <string.h>
#include "solver.h"

/* Решает одну строку и печатает результат. Возвращает 0, если решение найдено. */
static int solve_line(const char *line)
{
    char answer[256];

    if (solve_puzzle(line, answer, sizeof(answer)) != 0) {
        printf("%s -> решения нет\n", line);
        return 1;
    }
    printf("%s\n", answer);
    return 0;
}

int main(int argc, char **argv)
{
    char line[256];
    int failed = 0;

    /* ребус в аргументе: ./rebus "SEND + MORE = MONEY" */
    if (argc > 1)
        return solve_line(argv[1]);

    /* без аргументов читаем ребусы из stdin по одному в строке */
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        failed += solve_line(line);
    }
    return failed ? 1 : 0;
}
