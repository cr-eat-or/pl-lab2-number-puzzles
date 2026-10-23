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

//Состояние перебора. Буквы пронумерованы в порядке обработки (по убыванию |coef|)
typedef struct {
    int n;                               //число букв
    long long coef[MAX_LETTERS];         //коэффициент k-й буквы
    int lowest[MAX_LETTERS];             //наименьшая допустимая цифра: 1 для ведущей буквы, иначе 0
    long long min_rest[MAX_LETTERS + 1]; //наименьший вклад букв k..n-1 в сумму
    long long max_rest[MAX_LETTERS + 1]; //наибольший вклад букв k..n-1 в сумму
    int digit[MAX_LETTERS];              //цифра k-й буквы
} Search;

//Упорядочивает буквы по убыванию |coef| (order[k] - номер буквы в p.letters) и готовит границы.
//Границы считаются без учёта различности цифр, поэтому лишь отсекают заведомо безнадёжные ветки
static void prepare_search(const Puzzle *p, const long long *coef, int *order, Search *s)
{
    s->n = p->letter_count;
    for (int i = 0; i < s->n; i++)
        order[i] = i;

    //сортировка вставками: букв не больше 10
    for (int i = 1; i < s->n; i++) {
        int cur = order[i];
        long long cur_abs = coef[cur] < 0 ? -coef[cur] : coef[cur];
        int j = i - 1;
        while (j >= 0 && (coef[order[j]] < 0 ? -coef[order[j]] : coef[order[j]]) < cur_abs) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = cur;
    }

    for (int k = 0; k < s->n; k++) {
        s->coef[k] = coef[order[k]];
        s->lowest[k] = p->is_leading[order[k]] ? 1 : 0;
    }

    s->min_rest[s->n] = 0;
    s->max_rest[s->n] = 0;
    for (int k = s->n - 1; k >= 0; k--) {
        long long at_lowest = s->coef[k] * s->lowest[k];
        long long at_highest = s->coef[k] * 9;

        if (s->coef[k] >= 0) {
            s->min_rest[k] = s->min_rest[k + 1] + at_lowest;
            s->max_rest[k] = s->max_rest[k + 1] + at_highest;
        } else {
            s->min_rest[k] = s->min_rest[k + 1] + at_highest;
            s->max_rest[k] = s->max_rest[k + 1] + at_lowest;
        }
    }
}

//Перебор с возвратом: буквам по очереди назначаем ещё не занятые цифры (mask - занятые).
//sum - текущая сумма coef[k] * digit[k] по уже назначенным буквам.
//Если 0 не попадает в [sum + min_rest, sum + max_rest], ветку обрываем
static int search(Search *s, int pos, int mask, long long sum)
{
    if (sum + s->min_rest[pos] > 0 || sum + s->max_rest[pos] < 0)
        return 0;
    if (pos == s->n)
        return 1; //границы при pos == n равны нулю, значит sum == 0

    for (int d = s->lowest[pos]; d < 10; d++) {
        if (mask & (1 << d))
            continue;
        s->digit[pos] = d;
        if (search(s, pos + 1, mask | (1 << d), sum + s->coef[pos] * d))
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
    int order[MAX_LETTERS];
    Search st;

    if (puzzle_parse(input, &p) != 0)
        return -1;

    for (int i = 0; i < p.letter_count; i++)
        index_of[p.letters[i] - 'A'] = i;
    build_coefficients(&p, index_of, coef);

    prepare_search(&p, coef, order, &st);
    if (!search(&st, 0, 0, 0))
        return -1;
    for (int k = 0; k < st.n; k++)
        digits[order[k]] = st.digit[k];

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
