#pragma once
#include <stdbool.h>
#include "../blocks/dir_block.h"

#pragma once

// osFile
struct osFile;
typedef struct osFile osFile;

// Definición de osFile
struct osFile{
    Index_block* index_block;
    unsigned long int index_blocks_used; // La cantidad de bloques indice que usa el archivo
    unsigned long int data_blocks_used; // La cantidad de bloques de datos que usa el archivo
    unsigned long int indirect_blocks_used; // La cantidad de bloques de DIS que usa el archivo
};

// Inicializa un os file en formato r o w
osFile* os_open(char* path, char mode);
int os_close(osFile* file_desc);

void os_mount(char* diskname);
void os_bitmap(unsigned num, bool hex);
int os_exists(char* path);
void os_ls(char* path);
char* dir_name_from_path(char* path);
long unsigned int find_block_by_path(char* path);
int os_rm(char* path);