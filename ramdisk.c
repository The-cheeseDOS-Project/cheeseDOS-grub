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

#include <stddef.h>
#include <stdint.h>

#define RAMDISK_SIZE 1024
#define RAMDISK_FILENAME_MAX 8

typedef enum {
    RAMDISK_INODE_TYPE_UNUSED = 0,
    RAMDISK_INODE_TYPE_FILE = 1,
    RAMDISK_INODE_TYPE_DIR = 2,
} ramdisk_inode_type_t;

typedef struct {
    uint32_t inode_no;
    ramdisk_inode_type_t type;
    uint32_t size;
    uint32_t parent_inode_no;
    char name[RAMDISK_FILENAME_MAX];
    uint8_t data[256];
} ramdisk_inode_t;

static ramdisk_inode_t inodes[32];

static int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

static void *mem_copy(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void ramdisk_init() {
    for (int i = 0; i < 32; i++) {
        inodes[i].inode_no = i;
        inodes[i].type = RAMDISK_INODE_TYPE_UNUSED;
        inodes[i].size = 0;
        inodes[i].parent_inode_no = 0;
        for (int j = 0; j < RAMDISK_FILENAME_MAX; j++) {
            inodes[i].name[j] = 0;
        }
        for (int j = 0; j < 256; j++) {
            inodes[i].data[j] = 0;
        }
    }
    inodes[0].type = RAMDISK_INODE_TYPE_DIR;
    inodes[0].inode_no = 0;
    inodes[0].parent_inode_no = 0;
    const char root_name[] = "/";
    for (size_t i = 0; i < sizeof(root_name); i++) inodes[0].name[i] = root_name[i];
}

ramdisk_inode_t* ramdisk_iget(uint32_t inode_no) {
    if (inode_no >= 32) return NULL;
    if (inodes[inode_no].type == RAMDISK_INODE_TYPE_UNUSED) return NULL;
    return &inodes[inode_no];
}

int ramdisk_create_file(uint32_t parent_dir_inode_no, const char *filename) {
    if (!filename) return -1;
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type != RAMDISK_INODE_TYPE_UNUSED) {
            if (strcmp(inodes[i].name, filename) == 0 && inodes[i].parent_inode_no == parent_dir_inode_no) return -1;
        }
    }
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type == RAMDISK_INODE_TYPE_UNUSED) {
            inodes[i].type = RAMDISK_INODE_TYPE_FILE;
            inodes[i].parent_inode_no = parent_dir_inode_no;
            size_t len = 0;
            while (filename[len] && len < RAMDISK_FILENAME_MAX - 1) {
                inodes[i].name[len] = filename[len];
                len++;
            }
            inodes[i].name[len] = 0;
            inodes[i].size = 0;
            return 0;
        }
    }
    return -1;
}

int ramdisk_create_dir(uint32_t parent_dir_inode_no, const char *dirname) {
    if (!dirname) return -1;
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type != RAMDISK_INODE_TYPE_UNUSED) {
            if (strcmp(inodes[i].name, dirname) == 0 && inodes[i].parent_inode_no == parent_dir_inode_no) return -1;
        }
    }
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type == RAMDISK_INODE_TYPE_UNUSED) {
            inodes[i].type = RAMDISK_INODE_TYPE_DIR;
            inodes[i].parent_inode_no = parent_dir_inode_no;
            size_t len = 0;
            while (dirname[len] && len < RAMDISK_FILENAME_MAX - 1) {
                inodes[i].name[len] = dirname[len];
                len++;
            }
            inodes[i].name[len] = 0;
            inodes[i].size = 0;
            return 0;
        }
    }
    return -1;
}

int ramdisk_remove_file(uint32_t parent_dir_inode_no, const char *filename) {
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type != RAMDISK_INODE_TYPE_UNUSED && inodes[i].parent_inode_no == parent_dir_inode_no) {
            if (strcmp(inodes[i].name, filename) == 0) {
                inodes[i].type = RAMDISK_INODE_TYPE_UNUSED;
                inodes[i].size = 0;
                inodes[i].parent_inode_no = 0;
                for (int j = 0; j < RAMDISK_FILENAME_MAX; j++) inodes[i].name[j] = 0;
                for (int j = 0; j < 256; j++) inodes[i].data[j] = 0;
                return 0;
            }
        }
    }
    return -1;
}

int ramdisk_readfile(ramdisk_inode_t *file, uint32_t offset, uint32_t size, char *buffer) {
    if (!file || !buffer) return -1;
    if (offset > file->size) return 0;
    if (offset + size > file->size) size = file->size - offset;
    mem_copy(buffer, &file->data[offset], size);
    return size;
}

int ramdisk_writefile(ramdisk_inode_t *file, uint32_t offset, uint32_t size, const char *buffer) {
    if (!file || !buffer) return -1;
    if (file->type != RAMDISK_INODE_TYPE_FILE) return -1;

    if (offset > file->size) return -1;

    if (offset + size > sizeof(file->data)) {
        if (offset >= sizeof(file->data)) return -1;
        size = sizeof(file->data) - offset;
    }

    mem_copy(&file->data[offset], buffer, size);

    if (offset + size > file->size) {
        file->size = offset + size;
    }

    return size;
}

typedef void (*ramdisk_readdir_callback)(const char *name, uint32_t inode_no);

void ramdisk_readdir(ramdisk_inode_t *dir, ramdisk_readdir_callback cb) {
    if (!dir || dir->type != RAMDISK_INODE_TYPE_DIR || !cb) return;
    for (int i = 0; i < 32; i++) {
        if (inodes[i].type != RAMDISK_INODE_TYPE_UNUSED && inodes[i].parent_inode_no == dir->inode_no) {
            cb(inodes[i].name, inodes[i].inode_no);
        }
    }
}
