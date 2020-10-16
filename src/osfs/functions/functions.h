#pragma once

// Modifica el bitmap correspondiente al bloque block_num al valor value (1 o 0)
void modify_bitmap(unsigned long int block_num, int value);
long unsigned int find_block_by_path(char* path);
void create_directory(unsigned long int parent_block, unsigned int empty_block, int empty_entry, unsigned char* dir_name);
void read_from_position(long int start, unsigned char* buffer, long int buff_size);
void print_hex_buffer(unsigned char* hex_buffer, unsigned char* buffer, long int buff_size);
void print_binary_buffer(unsigned char* buffer, long int buff_size);
unsigned long int find_empty_block();
long unsigned int find_parent_block_by_path(char* path);