#include "dir_block.h"
#include "../API/os_API.h"
#include "../functions/functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern char* diskname;

Dir_block* dir_block_init(unsigned int block_num)
{
  Dir_block* dir_block = malloc(sizeof(Dir_block));
  unsigned int entry_size = 32;
  unsigned int total_entries = 64;

  for (unsigned int i = 0; i < total_entries; i++)
  {
      dir_block->entries[i] = block_num * 2048 + entry_size * i;
  }

  return dir_block;
}

Dir_block_entry* dir_block_entry_init(Dir_block* dir_block, unsigned int entry_num)
{
    Dir_block_entry* dir_block_entry = malloc(sizeof(Dir_block_entry));

    // Leemos los 32B de la entry
    unsigned int entry_ptr = dir_block->entries[entry_num];
    long int buff_size = 32;
    unsigned char buffer[buff_size];
    read_from_position(entry_ptr, buffer, buff_size);

    // Sacamos los primeros 2 bits: valido, archivo u directorio
    dir_block_entry->valid = buffer[0] >> 6;

    // Sacamos los bits 2 a 22: numero de un bloque indice o directorio
    // Para el primer byte sin los primeros 2 bits hay que hacer un AND con 0011 1111
    unsigned long int block_num = 0;
    block_num = (block_num + (buffer[0] & 0b00111111)) << 8;
    block_num = (block_num + buffer[1]) << 8;
    block_num = (block_num + buffer[2]);

    dir_block_entry->block_num = block_num;

    char name[29];
    // Sacamos los ultimos 29 bytes: nombre del archivo/directorio
    for (int i = 0; i < 29; i++)
    {
      name[i] = buffer[i+3];
    }
    strcpy(dir_block_entry->name, name);

    return dir_block_entry;
}

Index_block* index_block_init(unsigned int block_number, int first){
    Index_block* index_block = malloc(sizeof(Index_block));

    // Evaluamos si es el primero o no
    index_block -> position = first;
    long int buff_size; 
    unsigned long int pointer;

    if(first==1){
      index_block -> num_pointers = 509;
       // Leemos el primer byte
      buff_size = 1;
      unsigned char buffer1[buff_size];
      read_from_position(2048*block_number, buffer1, buff_size);
      index_block -> num_hardlinks = buffer1[0];

      // Leemos los siguientes 7 bytes
      buff_size = 7;
      unsigned char buffer2[buff_size];
      read_from_position(2048*block_number + 1, buffer2, buff_size);
      unsigned long long file_size = 0;
      file_size = (file_size + buffer2[0]) << 8;
      file_size = (file_size + buffer2[1]) << 8;
      file_size = (file_size + buffer2[2]) << 8;
      file_size = (file_size + buffer2[3]) << 8;
      file_size = (file_size + buffer2[4]) << 8;
      file_size = (file_size + buffer2[5]) << 8;
      file_size = (file_size + buffer2[6]);
      index_block -> file_size = file_size;

      // Leemos los siguientes 2036 bytes
      buff_size = 2036;
      unsigned char buffer3[buff_size];
      read_from_position(2048*block_number + 8, buffer3, buff_size);
      for(int i=0; i<509; i++){
        pointer = 0;
        pointer = (pointer + buffer3[4*i + 0]) << 8;
        pointer = (pointer + buffer3[4*i + 1]) << 8;
        pointer = (pointer + buffer3[4*i + 2]) << 8;
        pointer = (pointer + buffer3[4*i + 3]);
        index_block -> pointers[i] = pointer;
      }
    }
    else{
      index_block -> num_pointers = 511;
      // Leemos los siguientes 8 + 2036 bytes
      buff_size = 2044;
      unsigned char buffer5[buff_size];
      read_from_position(2048*block_number, buffer5, buff_size);
      for(int i=0; i<511; i++){
        pointer = 0;
        pointer = (pointer + buffer5[4*i + 0]) << 8;
        pointer = (pointer + buffer5[4*i + 1]) << 8;
        pointer = (pointer + buffer5[4*i + 2]) << 8;
        pointer = (pointer + buffer5[4*i + 3]);
        index_block -> pointers[i] = pointer;
      }
    }

    // Leemos los últimos 4 bytes
    buff_size = 4;
    unsigned char buffer4[buff_size];
    read_from_position(2048*block_number + 2044, buffer4, buff_size);
    index_block -> next_index = buffer4[0] | buffer4[1] | buffer4[2] | buffer4[3];
    pointer = 0;
    pointer = (pointer + buffer4[0]) << 8;
    pointer = (pointer + buffer4[1]) << 8;
    pointer = (pointer + buffer4[2]) << 8;
    pointer = (pointer + buffer4[3]);
    index_block -> next_index = pointer;
    return index_block;
}

Data_block* data_block_init(unsigned int block_number){
  Data_block* data_block = malloc(sizeof(Data_block));

  data_block->block_num = block_number;
  unsigned int buff_size = 2048;
  unsigned char buffer[buff_size];
  read_from_position(2048*block_number, buffer, buff_size);

  for (unsigned int i = 0; i < buff_size; i++)
  {
      data_block -> data[i] = buffer[i];
  }
  return data_block;
}

Dis_block* dis_block_init(unsigned int block_number){
  Dis_block* dis_block = malloc(sizeof(Dis_block));

  int buff_size = 2048;
  unsigned char buffer[buff_size];
  read_from_position(2048*block_number, buffer, buff_size);
  unsigned long int pointer;
  for(int i=0; i<(2048/4); i++){
        pointer = 0;
        pointer = (pointer + buffer[4*i + 0]) << 8;
        pointer = (pointer + buffer[4*i + 1]) << 8;
        pointer = (pointer + buffer[4*i + 2]) << 8;
        pointer = (pointer + buffer[4*i + 3]);
        dis_block -> pointers[i] = pointer;
      }
  return dis_block;
}

unsigned long int find_block_by_name(unsigned int curr_block_num, char* name)
{
  Dir_block* dir_block = dir_block_init(curr_block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    if(strcmp(name, dir_entry->name) == 0 && dir_entry->valid != 0) {
      unsigned long int block_num = dir_entry->block_num;
      free(dir_entry);
      free(dir_block);
      return block_num;
    }
    free(dir_entry);
  }
  free(dir_block);
  return -1;
}

unsigned long int find_entry_num_by_name(unsigned int curr_block_num, char* name)
{
  Dir_block* dir_block = dir_block_init(curr_block_num);
  for (unsigned long int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);

    //  Verifico que sea el nombre correcto y que apunte a un directorio
    if(strcmp(name, dir_entry->name) == 0 && dir_entry->valid != 0) {
      free(dir_entry);
      free(dir_block);
      return i;
    }
    free(dir_entry);
  }
  free(dir_block);
  return -1;
}

void print_files_from_dir(unsigned int dir_block_num)
{
  Dir_block*  dir_block = dir_block_init(dir_block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);

    //  Verifico que sea el nombre correcto y que apunte a un archivo
    if(dir_entry->valid != 0) {
    }
    free(dir_entry);
  }
  free(dir_block);
  return;
}

void print_all_entries_from_dir(unsigned int curr_block_num)
{
  Dir_block*  dir_block = dir_block_init(curr_block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    printf("Entry %i: %s |Block: %i |valid: %i\n", i, dir_entry->name, dir_entry->block_num,dir_entry->valid);
    free(dir_entry);
  }
  free(dir_block);
  return;
}

int find_empty_entry (unsigned long int block_num)
{
  Dir_block*  dir_block = dir_block_init(block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    if (!dir_entry->valid) //significa que está vacío
     {
      free(dir_entry);
      free(dir_block);
      return i;
    } 
      
    free(dir_entry);
  }
  free(dir_block);
  return -1;
}
