#include <stdio.h>
#include <string.h>
#include "puzzle.h"
#include "solver.h"

//Значение слова при текущих цифрах: схема Горнера, без промежуточной строки
static long long word_value(const char *word, const int *index_of, const int *digits)
{
    long long value = 0;

    for (const char *c = word; *c; c++)
        value = value * 10 + digits[index_of[*c - 'A']];
    return value;
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
static int is_solution(const Puzzle *p, const int *index_of, const int *digits)
{
    long long sum = 0;

    for (int i = 0; i < p->word_count; i++)
        sum += word_value(p->words[i], index_of, digits);
    return sum == word_value(p->result, index_of, digits);
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
    int index_of[26]; //буква -> её номер в p.letters, строится один раз

    if (puzzle_parse(input, &p) != 0)
        return -1;

    for (int i = 0; i < p.letter_count; i++)
        index_of[p.letters[i] - 'A'] = i;

    //перебираем
    do {
        if (!all_different(digits, p.letter_count))
            continue;
        if (!no_leading_zero(&p, digits))
            continue;
        if (!is_solution(&p, index_of, digits))
            continue;

        //ответ
        char line[256] = "";
        char num[32];
        for (int i = 0; i < p.word_count; i++) {
            sprintf(num, "%lld", word_value(p.words[i], index_of, digits));
            if (i > 0)
                strcat(line, " + ");
            strcat(line, num);
        }
        sprintf(num, "%lld", word_value(p.result, index_of, digits));
        strcat(line, " = ");
        strcat(line, num);

        if (strlen(line) + 1 > out_size)
            return -1;
        strcpy(out, line);
        return 0;
    } while (next_digits(digits, p.letter_count));

    return -1;
}
