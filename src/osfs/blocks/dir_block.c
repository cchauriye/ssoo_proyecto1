#include "dir_block.h"
#include "../API/os_API.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern char* diskname;

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
      //printf("%c", dir_block_entry->name[i]);
    }
    //printf("\n");

    return dir_block_entry;
}

unsigned int find_dir_entry_by_name(unsigned int curr_block_num, char* name)
{
  Dir_block* dir_block = dir_block_init(curr_block_num);
  //printf("estoy buscando el name: %s \n", name);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    //printf("name del entry %i es %s: \n", i, dir_entry->name);
    //printf("\n");

    //  Verifico que sea el nombre correcto y que apunte a un directorio
    if(strcmp(name, dir_entry->name) == 0 && dir_entry->valid==2) {
      //printf("lo encontre!\n");
      unsigned int block_num = dir_entry->block_num;
      free(dir_entry);
      free(dir_block);
      return block_num;
    }
    free(dir_entry);
  }
  free(dir_block);
  return -1;
}

void print_files_from_dir(unsigned int curr_block_num, char* name)
{
  Dir_block*  dir_block = dir_block_init(curr_block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);

    //  Verifico que sea el nombre correcto y que apunte a un archivo
    if(dir_entry->valid==1) {
      printf("Entry %i: %s\n", i, dir_entry->name);
    }
    free(dir_entry);
  }
  free(dir_block);
  return;
}

// Imprime los nombres de todas las entradas de un bloque
void print_all_entries_from_dir(unsigned int curr_block_num)
{
  Dir_block*  dir_block = dir_block_init(curr_block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    printf("Entry %i: %s |Block: %i\n", i, dir_entry->name, dir_entry->block_num);
    free(dir_entry);
  }
  free(dir_block);
  return;
}

int find_empty_entry (unsigned int block_num)
{
  Dir_block*  dir_block = dir_block_init(block_num);
  for (int i = 0; i < 64; i++)
  {
    Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
    if (!dir_entry->valid) //significa que está vacío
     {
      printf("La entrada %i del bloque %i está vacía\n", i, block_num);
      free(dir_entry);
      free(dir_block);
      return i;
    } 
      
    free(dir_entry);
  }
  free(dir_block);
  return -1;
}

void create_directory(unsigned int parent_block, unsigned int empty_block, int empty_entry, char* dir_name)
{

  // Vamos a editar el bitmap (bloque 65)
  unsigned int start = 2048 + empty_block/8; // Donde parte el byte que queremos modificar
  int bit_offset = empty_block % 8; // Resto es 1
  int z = 128 >> bit_offset;
  printf("Start: %i\n", start);
  printf("z: %i\n", z);

  // Vamos a leer el Byte que queremos modificar
  long int buff_size = 1;
  unsigned char buffer[buff_size];
  read_from_position(start, buffer, buff_size);

  // Modificamos el bit que queremos
  unsigned char byte_to_modify = buffer[0];
  printf("El byte antes es: %i\n", byte_to_modify);
  byte_to_modify = byte_to_modify | z;
  buffer[0] = byte_to_modify;
  printf("El nuevo byte es: %i\n", byte_to_modify);
  FILE * pFile;
  pFile = fopen(diskname, "wb");
  fseek(pFile, start, SEEK_SET);
  fwrite(buffer, 1, 1, pFile);
  // // Editar directorio padre. El start es en la entrada.
  // // Escribimos el tipo (2):
  // start = 2048*8*parent_block +  32*8*empty_entry;
  // fseek(pFile, start, SEEK_SET);
  // value = 2;
  // fwrite((const void*) & value, sizeof(int), 1, pFile);

  // // Escribimos puntero al bloque directorio vacío
  // start = start + 2;
  // fseek(pFile, start, SEEK_SET);
  // unsigned int num = 2048*8 * empty_block;  //(revisar que sean 22 bits)
  // fwrite((const void*) & num, sizeof(unsigned int), 1, pFile);


  // //escribimos nombre directorio
  // start = start + 22;
  // fseek(pFile, start, SEEK_SET);
  // fwrite(dir_name, sizeof(dir_name), 1, pFile);
  
  fclose(pFile);
  return;
}


//https://stackoverflow.com/questions/41384262/convert-string-to-binary-in-c
char* stringToBinary(char* s)
 {
    if(s == NULL) return 0; /* no input string */
    size_t len = strlen(s);
    char *binary = malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

char* decimal_to_binary(int n)
{
  int c, d, t;
  char *p;

  t = 0;
  p = (char*)malloc(32+1);

  if (p == NULL)
    exit(EXIT_FAILURE);

  for (c = 31 ; c >= 0 ; c--)
  {
    d = n >> c;

    if (d & 1)
      *(p+t) = 1 + '0';
    else
      *(p+t) = 0 + '0';

    t++;
  }
  *(p+t) = '\0';

  return  p;
}

char* itob(int i) {
      static char bits[8] = {'0','0','0','0','0','0','0','0'};
      int bits_index = 7;
      while ( i > 0 ) {
         bits[bits_index--] = (i & 1) + '0';
         i = ( i >> 1);
      }
      return bits;
   }