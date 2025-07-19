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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEY_NULL        ((int)0x00)   
#define KEY_BACKSPACE   ((int)'\b')  
#define KEY_ENTER       ((int)'\n')  
#define KEY_LEFT        ((int)0x80)   
#define KEY_RIGHT       ((int)0x81)   
#define KEY_UP          ((int)0x82)   
#define KEY_DOWN        ((int)0x83)   
#define KEY_HOME        ((int)0x84)   
#define KEY_END         ((int)0x85)   

int keyboard_getchar();

#endif 
