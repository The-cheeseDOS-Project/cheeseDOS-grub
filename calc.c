/*
 * cheeseDOS - My x86 DOS
 * Copyright (C) 2025  Connor Thomson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#include "calc.h"
#include "string.h"
#include "vga.h"

extern int str_to_int(const char *str);

static void print_int(int value) {
    char buf[12];
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        putchar('0');
        return;
    }
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0 && (size_t)i < sizeof(buf) - 1) {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }
    if (is_negative) buf[i++] = '-';

    for (int j = i - 1; j >= 0; j--) putchar(buf[j]);
}

static int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

void calc_command(const char *expr) {
    int a = 0, b = 0;
    char op = 0;
    int pos = 0;
    int length = (int)kstrlen(expr);

    while (pos < length && expr[pos] == ' ') pos++;
    int start = pos;
    while (pos < length && expr[pos] >= '0' && expr[pos] <= '9') pos++;
    if (pos == start) {
        print("Invalid expression\n");
        return;
    }
    a = str_to_int(expr + start);

    while (pos < length && expr[pos] == ' ') pos++;
    if (pos >= length || !is_operator(expr[pos])) {
        print("Invalid expression\n");
        return;
    }
    op = expr[pos++];
    while (pos < length && expr[pos] == ' ') pos++;

    start = pos;
    while (pos < length && expr[pos] >= '0' && expr[pos] <= '9') pos++;
    if (pos == start) {
        print("Invalid expression\n");
        return;
    }
    b = str_to_int(expr + start);

    int result = 0;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) {
                print("Error: Division by zero\n");
                return;
            }
            result = a / b;
            break;
        default:
            print("Invalid operator\n");
            return;
    }
    print_int(result);
    print("\n");
}
