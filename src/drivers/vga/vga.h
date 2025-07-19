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

#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define COLOR_BLACK     0x0
#define COLOR_BLUE      0x9
#define COLOR_GREEN     0xA
#define COLOR_CYAN      0xB
#define COLOR_RED       0xC
#define COLOR_MAGENTA   0xD
#define COLOR_YELLOW    0xE
#define COLOR_WHITE     0xF

#define VGA_COLOR(fg, bg) ((bg << 4) | (fg & 0x0F))

void putchar(char c);
void print(const char* str);
void clear_screen();
void backspace();
void set_cursor(int position);
void set_cursor_pos(int pos);
int get_cursor();
void set_text_color(uint8_t fg, uint8_t bg);
int get_screen_width();  
int get_screen_height(); 

#endif 
