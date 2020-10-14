#pragma once
#include <stdbool.h>
#include "../blocks/dir_block.h"

// Definición de osFile
typedef struct osFile{
    Index_block* index_pointer;
} osFile;

void read_from_position(long int start, unsigned char* buffer, long int buff_size);
void print_hex_buffer(unsigned char* hex_buffer, unsigned char* buffer, long int buff_size);
void print_binary_buffer(unsigned char* buffer, long int buff_size);
void os_mount(char* diskname);
void os_bitmap(unsigned num, bool hex);
int os_exists(char* path);
void os_ls(char* path);
char* dir_name_from_path(char* path);