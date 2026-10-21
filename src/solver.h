#ifndef SOLVER_H
#define SOLVER_H

#include <stddef.h>

//Решает ребус вида "SEND + MORE = MONEY". При успехе записывает в out решение ("9567 + 1085 = 10652") и возвращает 0. Возвращает -1, если строка некорректна, решения нет или out мал
int solve_puzzle(const char *input, char *out, size_t out_size);

#endif
