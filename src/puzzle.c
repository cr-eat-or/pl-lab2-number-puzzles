#include <ctype.h>
#include <string.h>
#include "puzzle.h"

/* Добавляет букву в список уникальных, возвращает её индекс или -1, если букв слишком много. */
static int add_letter(Puzzle *p, char c)
{
    for (int i = 0; i < p->letter_count; i++) {
        if (p->letters[i] == c)
            return i;
    }
    if (p->letter_count >= MAX_LETTERS)
        return -1;
    p->letters[p->letter_count] = c;
    p->is_leading[p->letter_count] = 0;
    p->letter_count++;
    p->letters[p->letter_count] = '\0';
    return p->letter_count - 1;
}

/* Читает слово (заглавные буквы) из *s в dst, сдвигает *s. Возвращает 0 или -1. */
static int read_word(const char **s, char *dst, Puzzle *p)
{
    int len = 0;
    while (isspace((unsigned char)**s))
        (*s)++;
    while (**s >= 'A' && **s <= 'Z') {
        if (len >= MAX_LEN)
            return -1;
        int idx = add_letter(p, **s);
        if (idx < 0)
            return -1;
        if (len == 0)
            p->is_leading[idx] = 1;
        dst[len++] = **s;
        (*s)++;
    }
    dst[len] = '\0';
    while (isspace((unsigned char)**s))
        (*s)++;
    return len > 0 ? 0 : -1;
}

int puzzle_parse(const char *str, Puzzle *p)
{
    const char *s = str;

    memset(p, 0, sizeof(*p));

    /* слагаемые, разделённые '+' */
    for (;;) {
        if (p->word_count >= MAX_WORDS)
            return -1;
        if (read_word(&s, p->words[p->word_count], p) != 0)
            return -1;
        p->word_count++;
        if (*s == '+') {
            s++;
            continue;
        }
        break;
    }
    if (p->word_count < 2)
        return -1;

    /* знак '=' и результат */
    if (*s != '=')
        return -1;
    s++;
    if (read_word(&s, p->result, p) != 0)
        return -1;

    /* после результата допустимы только пробелы и конец строки */
    if (*s != '\0')
        return -1;
    return 0;
}
