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

#include <stdint.h>
#include "vga.h"
#include "keyboard.h" 

#define VGA_MEMORY ((uint16_t*)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static int cursor = 0;
static uint8_t current_fg = COLOR_WHITE;
static uint8_t current_bg = COLOR_BLACK;

static uint8_t get_vga_color() {
    return VGA_COLOR(current_fg, current_bg);
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void set_cursor(int position) {

    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x00); 
    outb(0x3D4, 0x0B);
    outb(0x3D5, 0x0F); 

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

void scroll_if_needed() {
    if (cursor < SCREEN_WIDTH * SCREEN_HEIGHT) return;

    for (int row = 1; row < SCREEN_HEIGHT; row++) {
        for (int col = 0; col < SCREEN_WIDTH; col++) {
            VGA_MEMORY[(row - 1) * SCREEN_WIDTH + col] =
                VGA_MEMORY[row * SCREEN_WIDTH + col];
        }
    }

    uint8_t color_byte = get_vga_color();
    for (int col = 0; col < SCREEN_WIDTH; col++) {
        VGA_MEMORY[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + col] =
            ' ' | (color_byte << 8);
    }

    cursor -= SCREEN_WIDTH;
}

void putchar(char c) {
    uint8_t color_byte = get_vga_color();

    if (c == '\n') {

        cursor += SCREEN_WIDTH - (cursor % SCREEN_WIDTH);
    } else if (c == '\b') {

        if (cursor > 0) cursor--;
        VGA_MEMORY[cursor] = ' ' | (color_byte << 8);
    } else {

        VGA_MEMORY[cursor++] = c | (color_byte << 8);
    }

    scroll_if_needed(); 
    set_cursor(cursor); 
}

void print(const char* str) {
    while (*str) putchar(*str++);
}

void clear_screen() {
    uint8_t color_byte = get_vga_color();
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VGA_MEMORY[i] = ' ' | (color_byte << 8);
    }
    cursor = 0;
    set_cursor(cursor);
}

void backspace() {
    putchar('\b');
}

int get_cursor() {
    return cursor;
}

void set_cursor_pos(int pos) {

    if (pos >= 0 && pos < SCREEN_WIDTH * SCREEN_HEIGHT) {
        cursor = pos;
        set_cursor(cursor);
    }
}

void set_text_color(uint8_t fg, uint8_t bg) {
    current_fg = fg & 0x0F; 
    current_bg = bg & 0x0F; 
}

int get_screen_width() {
    return SCREEN_WIDTH;
}

int get_screen_height() {
    return SCREEN_HEIGHT;
}
