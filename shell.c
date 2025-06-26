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

#include "shell.h"
#include "vga.h"
#include "keyboard.h"

#define MAX_CMD_LEN 64
#define HISTORY_SIZE 32

static int prompt_start_vga_pos;

static char history[HISTORY_SIZE][MAX_CMD_LEN];
static int history_count = 0;
static int history_pos = 0;
static int history_view_pos = -1;

static int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

static int strncmp(const char *a, const char *b, unsigned int n) {
    while (n && *a && (*a == *b)) {
        a++;
        b++;
        n--;
    }
    if (n == 0) return 0;
    return (unsigned char)*a - (unsigned char)*b;
}

static unsigned int strlen(const char *s) {
    unsigned int len = 0;
    while (s[len]) len++;
    return len;
}

static char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

static char *strncpy(char *dest, const char *src, unsigned int n) {
    unsigned int i = 0;
    while (i < n && src[i]) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i++] = '\0';
    }
    return dest;
}

static void print_prompt() {
    print("> ");
    prompt_start_vga_pos = get_cursor();
}

static void add_history(const char *cmd) {
    if (cmd[0] == '\0') return;
    if (history_count < HISTORY_SIZE) {
        strncpy(history[history_count], cmd, MAX_CMD_LEN - 1);
        history[history_count][MAX_CMD_LEN - 1] = '\0';
        history_count++;
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i-1], history[i]);
        }
        strncpy(history[HISTORY_SIZE - 1], cmd, MAX_CMD_LEN - 1);
        history[HISTORY_SIZE - 1][MAX_CMD_LEN - 1] = '\0';
    }
    history_pos = history_count;
    history_view_pos = -1;
}

static void clear_input_line(int len) {
    set_cursor_pos(prompt_start_vga_pos);
    for (int i = 0; i < len; i++) {
        putchar(' ');
    }
    set_cursor_pos(prompt_start_vga_pos);
}

static void load_history_line(char *input, int *idx, int *cursor_index, int pos) {
    if (pos < 0 || pos >= history_count) return;
    clear_input_line(*idx);
    strncpy(input, history[pos], MAX_CMD_LEN - 1);
    input[MAX_CMD_LEN - 1] = '\0';
    *idx = strlen(input);
    *cursor_index = *idx;
    print(input);
}

void shell_execute(const char* cmd) {
    if (cmd[0] == '\0') return;
    if (strcmp(cmd, "help") == 0) {
        print("Commands: help, clear, print, version, hello.\n");
    }
    else if (strcmp(cmd, "version") == 0) {
        print("cheeseDOS alpha\n");
    }
    else if (strcmp(cmd, "hello") == 0) {
        print("Hello, world!\n");
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if (strncmp(cmd, "print ", 5) == 0) {
        print(cmd + 5);
        print("\n");
    }
    else {
        print(cmd);
        print(": command not found\n");
    }
}

void shell_run() {
    char input[MAX_CMD_LEN] = {0};
    int idx = 0;
    int cursor_index = 0;

    print_prompt();

    while (1) {
        int c = keyboard_getchar();

        if (c == KEY_LEFT) {
            if (cursor_index > 0) {
                cursor_index--;
                set_cursor_pos(prompt_start_vga_pos + cursor_index);
            }
            continue;
        }

        if (c == KEY_RIGHT) {
            if (cursor_index < idx) {
                cursor_index++;
                set_cursor_pos(prompt_start_vga_pos + cursor_index);
            }
            continue;
        }

        if (c == KEY_UP) {
            if (history_count == 0) continue;
            if (history_view_pos == -1) {
                history_view_pos = history_count - 1;
            } else if (history_view_pos > 0) {
                history_view_pos--;
            }
            load_history_line(input, &idx, &cursor_index, history_view_pos);
            continue;
        }

        if (c == KEY_DOWN) {
            if (history_count == 0) continue;
            if (history_view_pos == -1) continue;
            if (history_view_pos < history_count - 1) {
                history_view_pos++;
                load_history_line(input, &idx, &cursor_index, history_view_pos);
            } else {
                clear_input_line(idx);
                idx = 0;
                cursor_index = 0;
                input[0] = '\0';
                set_cursor_pos(prompt_start_vga_pos);
                history_view_pos = -1;
            }
            continue;
        }

        if (c == '\n') {
            input[idx] = '\0';
            putchar('\n');
            add_history(input);
            shell_execute(input);
            idx = 0;
            cursor_index = 0;
            input[0] = '\0';
            print_prompt();
            continue;
        }

        if (c == '\b') {
            if (cursor_index > 0) {
                for (int i = cursor_index - 1; i < idx - 1; i++) {
                    input[i] = input[i + 1];
                }
                idx--;
                cursor_index--;
                set_cursor_pos(prompt_start_vga_pos + cursor_index);
                for (int i = cursor_index; i < idx; i++) {
                    putchar(input[i]);
                }
                putchar(' ');
                set_cursor_pos(prompt_start_vga_pos + cursor_index);
            }
            continue;
        }

        if (c >= 32 && c <= 126 && idx < MAX_CMD_LEN - 1) {
            for (int i = idx; i > cursor_index; i--) {
                input[i] = input[i - 1];
            }
            input[cursor_index] = c;
            idx++;
            cursor_index++;
            set_cursor_pos(prompt_start_vga_pos + cursor_index - 1);
            for (int i = cursor_index - 1; i < idx; i++) {
                putchar(input[i]);
            }
            set_cursor_pos(prompt_start_vga_pos + cursor_index);
            continue;
        }
    }
}
