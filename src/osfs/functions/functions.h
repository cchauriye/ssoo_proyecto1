#pragma once
#include "../API/os_API.h"

// Modifica el bitmap correspondiente al bloque block_num al valor value (1 o 0)
void modify_bitmap(unsigned long int block_num, int value);
long unsigned int find_block_by_path(char* path);
void create_dir_entry(unsigned long int parent_block, unsigned int empty_block, int empty_entry, unsigned char* dir_name, int type);
void read_from_position(long int start, unsigned char* buffer, long int buff_size);
void print_hex_buffer(unsigned char* hex_buffer, unsigned char* buffer, long int buff_size);
void print_binary_buffer(unsigned char* buffer, long int buff_size);
unsigned long int find_empty_block();
long unsigned int find_parent_block_by_path(char* path);
void new_index_block(unsigned long int empty_block);
char* find_name_by_path(char* path);
void write_entry_block(unsigned long int parent_block, unsigned int index_block, int empty_entry, unsigned char* name, int type);
int read_data_block(osFile* file_desc, Data_block* curr_data_block, unsigned char* small_buffer, int not_read_bytes, int nbytes);
