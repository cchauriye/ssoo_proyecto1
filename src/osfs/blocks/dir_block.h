#pragma once

// Bloque de directorio
struct dir_block;
typedef struct dir_block Dir_block;

struct dir_block
{
  unsigned int entries[64]; //Arreglo de punteros hacia las entradas
};

// Bitmap
struct bitmap;
typedef struct bitmap Bitmap;

struct bitmap
{
  unsigned int bloques[64]; //Arreglo de bloques
};

// Entrada de directorio
struct dir_block_entry;
typedef struct dir_block_entry Dir_block_entry;

struct dir_block_entry
{
  int valid; // 0 si es invalid, 1 si es un archivo, 2 si es otro directorio
  unsigned long int block_num; // Numero de bloque donde esta el indice del archivo o bloque directorio  
  char name[29];
};

// Bloque índice
struct index_block;
typedef struct index_block Index_block;

struct index_block{
  int position; // 1 si es que es el primero, cero de lo contrario
  int num_hardlinks; // 1 byte para la cantidad total de hardlinks
  int num_pointers; // La cantidad de punteros que hay en pointers (si es el primero hay 509 y los ultimos dos del arreglo estan vacios)
  unsigned long long int file_size; // 7 bytes para tamaño de archivo
  unsigned long int pointers[511]; // 509 0 511 punteros de 4 bytes a bloques de DIS
  unsigned long int next_index;
  unsigned long int block_num;
};

// Bloque de datos
struct data_block;
typedef struct data_block Data_block;

struct data_block
{
  char data[2048];
  unsigned long int block_num;
};

// Bloque de direccionamiento indirecto
struct dis_block;
typedef struct dis_block Dis_block;

struct dis_block
{
  unsigned long int pointers[2048/4];
  unsigned long int block_num;
};

// Inicializa un bloque directorio
Dir_block* dir_block_init(unsigned int block_number);

// Inicializa una entrada de directorio
Dir_block_entry* dir_block_entry_init(Dir_block* dir_block, unsigned int entry_num);

// Inicializa un bloque índice
Index_block* index_block_init(unsigned int block_number, int position);

// Inicializa un bloque de datos
Data_block* data_block_init(unsigned int block_number);

// Inicializa un bloque de direccionamiento indirecto simple
Dis_block* dis_block_init(unsigned int block_number);

// Busca un name en las entradas de directorio de un bloque de directorio, retorna el num de bloque o 0
unsigned long int find_block_by_name(unsigned int curr_block_num, char* name);

// Busca un name en las entradas de directorio de un bloque de directorio, retorna el num de entry o 0
unsigned long int find_entry_num_by_name(unsigned int curr_block_num, char* name);

// Imprimer todas las entradas de un directorio (incluyendo las vacias)
void print_all_entries_from_dir(unsigned int curr_block_num);

// Imprime las entradas de tipo archivo
void print_files_from_dir(unsigned int dir_block_num);

int find_empty_entry(unsigned long int block_num);

void save_dis_block(Dis_block* dis_block);
void save_index_block(Index_block* index_block);