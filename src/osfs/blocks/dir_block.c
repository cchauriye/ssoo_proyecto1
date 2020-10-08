#include "dir_block.h"
#include "../API/os_API.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/** Inicializa una lista vacía */
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
    // printf("el primer byte es: %i\n", buffer[0]);
    dir_block_entry->valid = buffer[0] >> 6;
    // printf("los primeros dos bits son: %u\n", dir_block_entry->valid);

    // Sacamos los bits 2 a 22: numero de un bloque indice o directorio
    // Para el primer byte sin los primeros 2 bits hay que hacer un AND con 0011 1111
    unsigned int first_chunk = buffer[0] & (unsigned int) (pow((double) 2, (double) 6) - 1);

    // Para concatenar el primer chunk hay que multiplicarlo por 2^16 (16 espacios a la izq) 
    first_chunk = first_chunk * (unsigned int) (pow((double) 2, (double) 16));

    // Para concatenar el segundo chunk hay que multiplicarlo por 2^8 (8 espacios a la izq)
    unsigned int second_chunk = buffer[1] * (unsigned int) (pow((double) 2, (double) 8));
    unsigned int third_chunk = buffer[2];

    // Ahora podemos sumar los chunks (concatenarlos)
    dir_block_entry->block_num = first_chunk + second_chunk + third_chunk;
    // printf("El puntero a bloque es %u\n", dir_block_entry->block_num);

    char name[29];
    // Sacamos los ultimos 29 bytes: nombre del archivo/directorio
    for (int i = 0; i < 29; i++)
    {
      name[i] = buffer[i+3];
    }
    strcpy(dir_block_entry->name, name);
    
    for (int i = 0; i < 29; i++)
    {
      printf("%c", dir_block_entry->name[i]);
    }
    printf("\n");

    return dir_block_entry;
}

unsigned int find_dir_entry_by_name(Dir_block* dir_block, char* name)
{
  printf("estoy buscando el name: %s \n", name);
  for (int i = 0; i < 32; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    printf("name del entry %i es %s: \n", i, dir_entry->name);
    // for (int j = 0; j < 29; j++)
    // {
    //   printf("%c", dir_entry->name[j]);
    // }
    printf("\n");
    if(strcmp(name, dir_entry->name) == 0) {
      printf("lo encontre!\n");
      unsigned int block_num = dir_entry->block_num;
      free(dir_entry);
      return block_num;
    }
    free(dir_entry);
  }
  return 0;
}