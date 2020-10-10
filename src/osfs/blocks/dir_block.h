// Indica que este archivo solo se debe incluir una vez en la compilación
#pragma once

struct dir_block;
typedef struct dir_block Dir_block;

// Un bloque de directorio (tiene muchas entradas)
struct dir_block
{
  unsigned int entries[64]; //Arreglo de punteros hacia las entradas
};

struct dir_block_entry;
typedef struct dir_block_entry Dir_block_entry;

// Una entrada de directorio
struct dir_block_entry
{
  int valid; // 0 si es invalid, 1 si es un archivo, 2 si es otro directorio
  unsigned int block_num; // Numero de bloque donde esta el indice del archivo o bloque directorio  
  char name[29];
};

/** Inicializa un bloque directorio */
Dir_block* dir_block_init(unsigned int block_number);
Dir_block_entry* dir_block_entry_init(Dir_block* dir_block, unsigned int entry_num);

/** Busca un name en las entradas de directorio de un bloque de directorio, retorna el num de bloque o 0*/
unsigned int find_dir_entry_by_name(unsigned int curr_block_num, char* name);

void print_all_entries_from_dir(unsigned int curr_block_num);
void print_files_from_dir(unsigned int curr_block_num, char* name);