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

struct index_block;
typedef struct index_block Index_block;

struct index_block{
  int position; // 1 si es que es el primero, cero de lo contrario
  int num_hardlinks; // 1 byte para la cantidad total de hardlinks
  long long int file_size; // 7 bytes para tamaño de archivo
  long int pointers[509]; // 509 punteros de 4 bytes a bloques de DIS
  long int next_index;
};

// Inicializa un bloque directorio
Dir_block* dir_block_init(unsigned int block_number);

// Inicializa una entrada de directorio
Dir_block_entry* dir_block_entry_init(Dir_block* dir_block, unsigned int entry_num);

// Inicializa un bloque índice
Index_block* index_block_init(unsigned int block_number, int position);

// Busca un name en las entradas de directorio de un bloque de directorio, retorna el num de bloque o 0
unsigned int find_dir_entry_by_name(unsigned int curr_block_num, char* name);

// Imprimer todas las entradas de un directorio (incluyendo las vacias)
void print_all_entries_from_dir(unsigned int curr_block_num);

// Imprime las entradas de tipo archivo
void print_files_from_dir(unsigned int curr_block_num, char* name);