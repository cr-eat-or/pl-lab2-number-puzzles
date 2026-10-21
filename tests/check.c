#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puzzle.h"
#include "solver.h"

/* Проверяет ответ вида "9567 + 1085 = 10652" для ребуса p:
   слова и числа совпадают по длине;
   одна буква соответствует одной цифре, разные буквы - разным цифрам;
   нет ведущих нулей;
   сумма слагаемых равна результату.
   
   Возвращает 1, если ответ верный. */
static int check_answer(const Puzzle *p, const char *answer)
{
    char copy[256];
    const char *nums[MAX_WORDS + 1];
    int count = 0;
    int digit_of[26], letter_of[10];
    long long sum = 0, result;

    if (strlen(answer) >= sizeof(copy))
        return 0;
    strcpy(copy, answer);

    /* разбиваем ответ на числа по пробелам, пропуская '+' и '=' */
    for (char *tok = strtok(copy, " "); tok; tok = strtok(NULL, " ")) {
        if (strcmp(tok, "+") == 0 || strcmp(tok, "=") == 0)
            continue;
        if (count > MAX_WORDS)
            return 0;
        nums[count++] = tok;
    }
    if (count != p->word_count + 1)
        return 0;

    for (int i = 0; i < 26; i++)
        digit_of[i] = -1;
    for (int i = 0; i < 10; i++)
        letter_of[i] = -1;

    for (int w = 0; w <= p->word_count; w++) {
        const char *word = (w < p->word_count) ? p->words[w] : p->result;
        const char *num = nums[w];

        if (strlen(word) != strlen(num) || num[0] == '0')
            return 0;
        for (size_t i = 0; word[i]; i++) {
            int letter = word[i] - 'A';
            int digit = num[i] - '0';
            if (digit < 0 || digit > 9)
                return 0;
            if (digit_of[letter] == -1 && letter_of[digit] == -1) {
                digit_of[letter] = digit;
                letter_of[digit] = letter;
            } else if (digit_of[letter] != digit || letter_of[digit] != letter) {
                return 0;
            }
        }
    }

    for (int w = 0; w < p->word_count; w++)
        sum += atoll(nums[w]);
    result = atoll(nums[p->word_count]);
    return sum == result;
}

/* Прогоняет все ребусы из файла. Возвращает число ошибок, -1 если файл не открылся. */
static int check_file(const char *path)
{
    FILE *f = fopen(path, "r");
    char line[256], answer[256];
    int total = 0, failed = 0;

    if (!f) {
        printf("Не удалось открыть %s\n", path);
        return -1;
    }
    while (fgets(line, sizeof(line), f)) {
        Puzzle p;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        total++;
        if (puzzle_parse(line, &p) != 0) {
            printf("FAIL (разбор): %s\n", line);
            failed++;
        } else if (solve_puzzle(line, answer, sizeof(answer)) != 0) {
            printf("FAIL (нет решения): %s\n", line);
            failed++;
        } else if (!check_answer(&p, answer)) {
            printf("FAIL (неверный ответ): %s -> %s\n", line, answer);
            failed++;
        } else {
            printf("OK: %s -> %s\n", line, answer);
        }
    }
    fclose(f);
    printf("%s: проверено %d, ошибок %d\n", path, total, failed);
    return failed;
}

int main(int argc, char **argv)
{
    int errors = 0;

    if (argc < 2) {
        printf("Использование: check <файл с ребусами> ...\n");
        return 2;
    }
    for (int i = 1; i < argc; i++) {
        int r = check_file(argv[i]);
        errors += (r < 0) ? 1 : r;
    }
    return errors ? 1 : 0;
}
