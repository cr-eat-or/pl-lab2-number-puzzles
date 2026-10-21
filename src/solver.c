#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puzzle.h"
#include "solver.h"

//Значение слова при текущих цифрах: ищем букву, собираем строку, переводим в число
static long long word_value(const char *word, const char *letters, const int *digits)
{
    char buf[MAX_LEN + 1];

    for (size_t i = 0; i < strlen(word); i++) {
        const char *pos = strchr(letters, word[i]);
        sprintf(buf + i, "%d", digits[pos - letters]);
    }
    return atoll(buf);
}

//Все ли цифры в наборе различны (проверка парами)
static int all_different(const int *digits, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (digits[i] == digits[j])
                return 0;
        }
    }
    return 1;
}

//Нет ли нуля у ведущих букв
static int no_leading_zero(const Puzzle *p, const int *digits)
{
    for (int i = 0; i < p->letter_count; i++) {
        if (p->is_leading[i] && digits[i] == 0)
            return 0;
    }
    return 1;
}

//Выполняется ли равенство при текущих цифрах
static int is_solution(const Puzzle *p, const int *digits)
{
    long long sum = 0;

    for (int i = 0; i < p->word_count; i++)
        sum += word_value(p->words[i], p->letters, digits);
    return sum == word_value(p->result, p->letters, digits);
}

//Следующий набор цифр, как счётчик. ret 0, когда перебор закончен
static int next_digits(int *digits, int n)
{
    for (int i = n - 1; i >= 0; i--) {
        if (digits[i] < 9) {
            digits[i]++;
            return 1;
        }
        digits[i] = 0;
    }
    return 0;
}

int solve_puzzle(const char *input, char *out, size_t out_size)
{
    Puzzle p;
    int digits[MAX_LETTERS] = {0};

    if (puzzle_parse(input, &p) != 0)
        return -1;

    //перебираем
    do {
        if (!all_different(digits, p.letter_count))
            continue;
        if (!no_leading_zero(&p, digits))
            continue;
        if (!is_solution(&p, digits))
            continue;

        //ответ
        char line[256] = "";
        char num[32];
        for (int i = 0; i < p.word_count; i++) {
            sprintf(num, "%lld", word_value(p.words[i], p.letters, digits));
            if (i > 0)
                strcat(line, " + ");
            strcat(line, num);
        }
        sprintf(num, "%lld", word_value(p.result, p.letters, digits));
        strcat(line, " = ");
        strcat(line, num);

        if (strlen(line) + 1 > out_size)
            return -1;
        strcpy(out, line);
        return 0;
    } while (next_digits(digits, p.letter_count));

    return -1;
}
