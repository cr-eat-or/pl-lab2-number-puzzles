#ifndef PUZZLE_H
#define PUZZLE_H

#define MAX_WORDS   7   /* слагаемых в левой части: от 2 до 7 */
#define MAX_LEN     16  /* максимальная длина слова */
#define MAX_LETTERS 10  /* разных букв не больше, чем цифр */

typedef struct {
    char words[MAX_WORDS][MAX_LEN + 1]; /* слагаемые */
    int  word_count;                    /* число слагаемых */
    char result[MAX_LEN + 1];           /* правая часть */
    char letters[MAX_LETTERS + 1];      /* уникальные буквы в порядке появления */
    int  letter_count;                  /* число уникальных букв */
    int  is_leading[MAX_LETTERS];       /* 1, если буква letters[i] стоит первой в слове */
} Puzzle;

/* Разбирает строку вида "SEND + MORE = MONEY".
   Возвращает 0 при успехе, -1 при ошибке формата. */
int puzzle_parse(const char *str, Puzzle *p);

#endif
