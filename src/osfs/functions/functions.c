#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h>
#include "functions.h"
#include "../API/os_API.h"
#include "../blocks/dir_block.h"

extern char* diskname;
extern unsigned int BLOCK_SIZE;

char* find_name_by_path(char* path){
    char *slash = path;
    char* dir_name;
    char* file_name;
    char* leftover;
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        //printf("leftover: %s\n", leftover);
        path = leftover;
        slash = leftover;
    };
    dir_name = slash;
return dir_name;
}

void modify_bitmap(unsigned long int block_num, int value){
    // Vamos a leer el Byte que queremos modificar
    int buff_size = 1;
    unsigned char buffer[buff_size];
    int start = 2048 + block_num / 8;
    read_from_position(start, buffer, buff_size);

    // Alteramos el bit correspondiente
    int bit_offset = block_num % 8;
    int z;

    if (value == 1){
        z = 128 >> bit_offset;
        buffer[0] = buffer[0] | z;
    }
    else {
        z = 255 - pow((double)2, (double)(7-bit_offset));
        buffer[0] = buffer[0] & z;
    }

    // Escribimos el bitmap
    FILE * pFile;
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 1, 1, pFile);
    fclose(pFile);
}

long unsigned int find_block_by_path(char* path){
    char path2[100];
    strcpy(path2, path);
    long unsigned int block_num = 0;
    // Extract the first token
    char* next_dir = strtok(path2, "/");
    // loop through the string to extract all other tokens
    while( next_dir != NULL) {
        // printf( "next_dir: %s\n", next_dir ); //printing each token
        block_num = find_block_by_name(block_num, next_dir);
        if (block_num == -1){
            return -1;
        }
        next_dir = strtok(NULL, "/");
    }
    return block_num;
}

// Imprimir buffer en binario
void print_binary_buffer(unsigned char* buffer, long int buff_size)
{
    // for (int i = 0; i < buff_size; i++) {
    //     printf("%i", buffer[i]);
    // }
    // printf("\n");
    for(int i = 0; i < buff_size; i++) {
        int z = 128, oct = buffer[i];
        while (z > 0)
        {
            if (oct & z)
                fprintf(stderr, "1");
            else
                fprintf(stderr, "0");
            z >>= 1;
        }
        fprintf(stderr, "\n");
    }
}

// Lee el .bin desde una posición dada
void read_from_position(long int start, unsigned char* buffer, long int buff_size)
{
    FILE* ptr = fopen(diskname, "rb");
    if (!ptr)
    {
        printf("Unable to open file!");
    }
    fseek(ptr, start, SEEK_SET);
    fread(buffer, sizeof(unsigned char), buff_size, ptr);
    fclose(ptr);
};

// Imprimir buffer en hexadecimal
void print_hex_buffer(unsigned char* hex_buffer, unsigned char* buffer, long int buff_size)
{ 
    for (int i = 0; i < buff_size; i++) {
        int dec_num = buffer[i];
        int j = 2*i;
        if (dec_num == 0) 
        {
            hex_buffer[j] = 48;
            hex_buffer[j+1] = 48;
        }
        while(dec_num != 0) 
        {    
            // temporary variable to store remainder 
            int temp  = 0; 
            
            // storing remainder in temp variable. 
            temp = dec_num % 16; 
            
            // check if temp < 10 
            if(temp < 10) 
            { 
                hex_buffer[j] = temp + 48;
            } 
            else
            { 
                hex_buffer[j] = temp + 55;
            } 
            j++; 
            dec_num = dec_num/16; 
        } 
    }
}

unsigned long int find_empty_block(){
    //Tenemos que encontrar un bloque vacío en el bitmap
    int found = 0;
    unsigned long int empty_block;
    long int buff_size = BLOCK_SIZE;
    unsigned char buffer[buff_size];
    int curr_bitmap_block_num = 1;
    while (curr_bitmap_block_num < 66 && found==0)
    {
        read_from_position(BLOCK_SIZE*curr_bitmap_block_num, buffer, buff_size); 
        int i = 0;
        while(i < buff_size && found==0) {
            int bit = 0;
            int z = 128, oct = buffer[i];
            int nada;
            while (z > 0)
            {
                if (oct & z){
                    nada = 10;
                }
                else{
                    // printf("Bloque vacío! Bloque de bitmap = %i, BYTE: %i, (valor de z: %i, bit: %i)\n", curr_bitmap_block_num, i, z, bit);
                    // Transformar num, i y z en el numero de bloque vacio
                    empty_block = 2048*(curr_bitmap_block_num-1)*8 + 8*(i) + bit;
                    // printf("Equivale al bloque numero: %i\n", empty_block);
                    found = 1;
                    break;
                }
                z >>= 1;
                bit++;
            }
        fprintf(stderr, "\n");
        i++;
        }     
    curr_bitmap_block_num ++;
    }
    return empty_block;
}

long unsigned int find_parent_block_by_path(char* path){
    char path2[100];
    strcpy(path2, path);
    long unsigned int prev_block_num = 0;
    long unsigned int block_num = 0;
    // Extract the first token
    char* next_dir = strtok(path2, "/");
    // loop through the string to extract all other tokens
    while( next_dir != NULL) {
        // printf( "next_dir: %s\n", next_dir ); //printing each token
        prev_block_num = block_num;
        block_num = find_block_by_name(block_num, next_dir);
        next_dir = strtok(NULL, "/");
    }
    return prev_block_num;
}

void create_dir_entry(unsigned long int parent_block, unsigned int empty_block, int empty_entry, unsigned char* name, int type)
{
    modify_bitmap(empty_block, 1);

    FILE * pFile;
    pFile = fopen(diskname, "r+");
    // Editar directorio padre. El start es en la entrada.
    // // Escribimos que es de tipo 1:
    unsigned long int start = 2048*parent_block +  32*empty_entry;
    fseek(pFile, start, SEEK_SET);
    long int value = pow((double)2, (double)(21 + type)); // 1 en la posición que queremos
    unsigned long int new_block_pointer = empty_block; 
    unsigned long int result = value | new_block_pointer;
    unsigned char buffer1[3];
    // printf(": %li\n", result);
    buffer1[0] = (result & 0b00000000111111110000000000000000) >> 16;
    buffer1[1] = (result & 0b00000000000000001111111100000000) >> 8;
    buffer1[2] = (result & 0b00000000000000000000000011111111);
    // printf("Número de bloque: \n%i \n%i \n%i\n", buffer1[0], buffer1[1], buffer1[2]);
    fwrite(buffer1, 1, 3, pFile);

    // // Escribimos nombre del archivo
    start = 2048*parent_block +  32*empty_entry + 3;
    fseek(pFile, start, SEEK_SET);
    fwrite(name, strlen(name), 1, pFile);
    fclose(pFile);
    return;
}

void new_index_block(unsigned long int empty_block){
    // Preparamos los primeros 8 Bytes: 1 para num de hardlinks, 7 para file size
    unsigned long int start = BLOCK_SIZE * empty_block;
    unsigned char buffer[8];
    buffer[0] = 0b00000001;
    for (int i = 1; i < 8; i++)
    {
        buffer[i] = 0b00000000;
    }
    FILE * pFile;
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 8, 1, pFile);
    fclose(pFile);
    return;
}

//escribir el nombre y puntero en la entrada del padre
void write_entry_block(unsigned long int parent_block, unsigned int index_block, int empty_entry, unsigned char* name, int type){
    FILE * pFile;
    pFile = fopen(diskname, "r+");
    //printf("nombre dentro: %s  , ", name);
    // Editar directorio padre. El start es en la entrada.
    // // Escribimos que es de tipo 1:
    //printf("Parent block en start: %i\n", parent_block);
    //printf("Empty entry en start: %i\n", empty_entry);
    unsigned long int start = 2048*parent_block +  32*empty_entry;
    fseek(pFile, start, SEEK_SET);
    long int value = pow((double)2, (double)(21 + type)); // 1 en la posición que queremos
    unsigned long int new_block_pointer = index_block; 
    unsigned long int result = value | new_block_pointer;
    unsigned char buffer1[3];
    // printf(": %li\n", result);
    buffer1[0] = (result & 0b00000000111111110000000000000000) >> 16;
    buffer1[1] = (result & 0b00000000000000001111111100000000) >> 8;
    buffer1[2] = (result & 0b00000000000000000000000011111111);
    // printf("Número de bloque: \n%i \n%i \n%i\n", buffer1[0], buffer1[1], buffer1[2]);
    fwrite(buffer1, 1, 3, pFile);

    // // Escribimos nombre del archivo
    start = 2048*parent_block +  32*empty_entry + 3;
    fseek(pFile, start, SEEK_SET);
    //printf("nombre: %s  , start: %i", name, start);
    fwrite(name, strlen(name), 1, pFile);
    fclose(pFile);
    return;
}