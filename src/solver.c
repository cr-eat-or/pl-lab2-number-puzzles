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

//Коэффициенты букв: сумма весов разрядов в слагаемых минус вес в результате.
//Равенство выполнено, когда сумма coef[i] * digits[i] равна нулю
static void build_coefficients(const Puzzle *p, const int *index_of, long long *coef)
{
    for (int i = 0; i < p->letter_count; i++)
        coef[i] = 0;

    for (int w = 0; w <= p->word_count; w++) {
        const char *word = (w < p->word_count) ? p->words[w] : p->result;
        long long weight = 1;

        for (int j = (int)strlen(word) - 1; j >= 0; j--) {
            int idx = index_of[word[j] - 'A'];
            if (w < p->word_count)
                coef[idx] += weight;
            else
                coef[idx] -= weight;
            weight *= 10;
        }
    }
}

//Перебор с возвратом: буквам по очереди назначаем ещё не занятые цифры (mask - занятые).
//sum - текущая сумма coef[i] * digits[i] по уже назначенным буквам
static int search(const Puzzle *p, const long long *coef, int pos, int mask, long long sum, int *digits)
{
    if (pos == p->letter_count)
        return sum == 0;

    for (int d = 0; d < 10; d++) {
        if (mask & (1 << d))
            continue;
        if (d == 0 && p->is_leading[pos])
            continue;
        digits[pos] = d;
        if (search(p, coef, pos + 1, mask | (1 << d), sum + coef[pos] * d, digits))
            return 1;
    }
    return 0;
}

int solve_puzzle(const char *input, char *out, size_t out_size)
{
    Puzzle p;
    int digits[MAX_LETTERS] = {0};
    int index_of[26]; //буква -> её номер в p.letters, строится один раз
    long long coef[MAX_LETTERS];

    if (puzzle_parse(input, &p) != 0)
        return -1;

    for (int i = 0; i < p.letter_count; i++)
        index_of[p.letters[i] - 'A'] = i;
    build_coefficients(&p, index_of, coef);

    if (!search(&p, coef, 0, 0, 0, digits))
        return -1;

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
}
