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
    unsigned long int index_block_num; // Bloque donde esta el primer index block
    unsigned long int index_blocks_used; // La cantidad de bloques indice que usa el archivo
    unsigned long int data_blocks_used; // La cantidad de bloques de datos que usa el archivo
    unsigned long int indirect_blocks_used; // La cantidad de bloques de DIS que usa el archivo

    // Info util para os_read
    unsigned long long int bytes_read; // La cantidad de bytes leidos por os_read
    unsigned long int index_blocks_read; // La cantidad de index blocks leidos por os_read
    unsigned long int dis_blocks_read; // La cantidad de dis blocks leidos por os_read
    unsigned long int data_blocks_read; // La cantidad de data blocks leidos por os_read
    int index_block_offset; // Offset en el ultimo index block leido (cantidad de ptrs que se leyeron completos)
    int dis_block_offset; // Offset en el ultimo dis block leido (cantidad de ptrs que se leyeron completos)
    int data_block_offset; // Offset en el ultimo data block leido (cantidad de bytes leidos de ese bloque)
};

// Inicializa un os file en formato r o w
osFile* os_open(char* path, char mode);
int os_close(osFile* file_desc);
int os_read(osFile* file_desc, void* buffer, int nbytes);
int os_write(osFile* file_desc, void* buffer, int nbytes);
void os_mount(char* diskname);
void os_bitmap(unsigned num, bool hex);
int os_exists(char* path);
void os_ls(char* path);
char* dir_name_from_path(char* path);
long unsigned int find_block_by_path(char* path);
int os_rm(char* path);
int os_mkdir(char* path);
void os_rmdir(char* path, bool recursive);
void os_hardlink(char*orig, char*dest);
int os_unload(char* orig, char* dest);