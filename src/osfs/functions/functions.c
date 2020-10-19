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
    int errnum;
    if (pFile == NULL) {
        errnum = errno;
        fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
        return;
    }
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
    int errnum;
    if (ptr == NULL) {
        errnum = errno;
        fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
    } 
    else 
    {
    fseek(ptr, start, SEEK_SET);
    fread(buffer, sizeof(unsigned char), buff_size, ptr);
    fclose(ptr);
    };
};

// Imprimir buffer en hexadecimal
void print_hex_buffer(unsigned char* buffer, long int buff_size)
{ 
    for (int i = 0; i < buff_size; i++) 
    {
        fprintf(stderr, "%02X\n", buffer[i]);
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
                    empty_block = 2048*(curr_bitmap_block_num-1)*8 + 8*(i) + bit;
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

    //manejo de error
    int errnum;
    if (pFile == NULL) {
        errnum = errno;
        fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
        return;
    }
    // Editar directorio padre. El start es en la entrada.
    // // Escribimos que es de tipo 1:
    unsigned long int start = 2048*parent_block +  32*empty_entry;
    fseek(pFile, start, SEEK_SET);
    long int value = pow((double)2, (double)(21 + type)); // 1 en la posición que queremos
    unsigned long int new_block_pointer = empty_block; 
    unsigned long int result = value | new_block_pointer;
    unsigned char buffer1[3];
    buffer1[0] = (result & 0b00000000111111110000000000000000) >> 16;
    buffer1[1] = (result & 0b00000000000000001111111100000000) >> 8;
    buffer1[2] = (result & 0b00000000000000000000000011111111);
    fwrite(buffer1, 1, 3, pFile);

    unsigned char buffer_set_zero[29];
    for (int i = 0; i < 29; i++)
    {
        buffer_set_zero[i] = 0;
    }
    //dejamos el nombre en 0
    start = 2048*parent_block +  32*empty_entry + 3;
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer_set_zero, 1, 29, pFile);

    // // Escribimos nombre del archivo
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

    //manejo de error
    int errnum;
    if (pFile == NULL) {
        errnum = errno;
        fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
        return;
    }

    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 8, 1, pFile);
    fclose(pFile);
    return;
}

//escribir el nombre y puntero en la entrada del padre
void write_entry_block(unsigned long int parent_block, unsigned int index_block, int empty_entry, unsigned char* name, int type){
    FILE * pFile;
    pFile = fopen(diskname, "r+");

    //manejo de error
    int errnum;
    if (pFile == NULL) {
        errnum = errno;
        fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
        return;
    }

    unsigned long int start = 2048*parent_block +  32*empty_entry;
    fseek(pFile, start, SEEK_SET);
    long int value = pow((double)2, (double)(21 + type)); // 1 en la posición que queremos
    unsigned long int new_block_pointer = index_block; 
    unsigned long int result = value | new_block_pointer;
    unsigned char buffer1[3];
    buffer1[0] = (result & 0b00000000111111110000000000000000) >> 16;
    buffer1[1] = (result & 0b00000000000000001111111100000000) >> 8;
    buffer1[2] = (result & 0b00000000000000000000000011111111);
    fwrite(buffer1, 1, 3, pFile);

    // // Escribimos nombre del archivo
    start = 2048*parent_block +  32*empty_entry + 3;
    fseek(pFile, start, SEEK_SET);
    fwrite(name, strlen(name), 1, pFile);
    fclose(pFile);
    return;
}

int read_data_block(osFile* file_desc, Data_block* curr_data_block, unsigned char* small_buffer, int not_read_bytes, int nbytes){
    
    int offset = file_desc->data_block_offset;
    int bytes_to_read;

    // Los not read bytes si caben en este DB
    if (BLOCK_SIZE - offset > not_read_bytes)
    {
        bytes_to_read = not_read_bytes;
        long unsigned int start = BLOCK_SIZE*curr_data_block->block_num + offset;
        FILE * pFile;
        pFile = fopen(diskname, "r");

        //manejo de error
        int errnum;
        if (pFile == NULL) {
            errnum = errno;
            fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
            return;
        }

        fseek(pFile, start, SEEK_SET);
        fread(small_buffer, bytes_to_read, 1, pFile);
        fclose(pFile);
        file_desc->data_block_offset += not_read_bytes;
    }
    else
    {
        bytes_to_read = BLOCK_SIZE - offset;
        // Leemos lo que queda de este DB
        long unsigned int start = BLOCK_SIZE*curr_data_block->block_num + offset;
        FILE * pFile;
        pFile = fopen(diskname, "r");

        //manejo de error
        int errnum;
        if (pFile == NULL) {
            errnum = errno;
            fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
            return;
        }

        fseek(pFile, start, SEEK_SET);
        fread(small_buffer, bytes_to_read, 1, pFile);
        fclose(pFile);

        file_desc->data_block_offset = 0;
        file_desc->data_blocks_read ++;
        file_desc->dis_block_offset ++;
    }
    file_desc->bytes_read += (unsigned long long int)bytes_to_read;
    return bytes_to_read;
} 

int write_data_block(osFile* file_desc, Data_block* curr_data_block, void* buffer, int not_written_bytes, int nbytes, int total_written_bytes){
   
    int offset = file_desc->data_block_offset;
    int bytes_to_write;

     // Los not written bytes si caben en este DB
    if (BLOCK_SIZE - offset > not_written_bytes)
    {
        bytes_to_write = not_written_bytes;
        unsigned char local_buffer[bytes_to_write];
        for (int i = 0; i < bytes_to_write; i++)
        {
            local_buffer[i] = ((unsigned char*)buffer)[i + total_written_bytes]; // Flaitismo puro
        }
        long unsigned int start = BLOCK_SIZE*curr_data_block->block_num + offset;
        FILE * pFile;
        pFile = fopen(diskname, "r+");

        //manejo de error
        int errnum;
        if (pFile == NULL) {
            errnum = errno;
            fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
            return;
        }

        fseek(pFile, start, SEEK_SET);
        fwrite(local_buffer, bytes_to_write, 1, pFile);
        fclose(pFile);
        file_desc->data_block_offset += bytes_to_write;
    }
    // Los not written bytes bytes no caben en este DB
    // Escribimos todo lo que queda de este DB
    else
    {
        bytes_to_write = BLOCK_SIZE - offset;
        unsigned char local_buffer[bytes_to_write];

        for (int i = 0; i < bytes_to_write; i++)
        {
            local_buffer[i] = ((unsigned char*)buffer)[i + total_written_bytes]; // Flaitismo puro
        }

        long unsigned int start = BLOCK_SIZE*curr_data_block->block_num + offset;
        FILE * pFile;
        pFile = fopen(diskname, "r+");

        //manejo de error
        int errnum;
        if (pFile == NULL) {
            errnum = errno;
            fprintf(stderr, "Error al leer el disco: %s\n", strerror( errnum ));
            return;
        }
        
        fseek(pFile, start, SEEK_SET);
        fwrite(local_buffer, bytes_to_write, 1, pFile);
        fclose(pFile);
        
        file_desc->data_block_offset = 0;
        file_desc->data_blocks_read ++;
        file_desc->dis_block_offset ++;
    }
    return bytes_to_write;
}

unsigned long int next_db(osFile* file_desc, Index_block* curr_index_block, Dis_block* curr_dis_block, Data_block* curr_data_block){

    // next_db(file_desc, curr_dis, curr_index, dis_block_offset): retorna block num del siguiente DB
    // Si quedan ptrs por leer en el DIS block --> retornar block num del siguiente ptr
    // Si no quedan ptrs por leer en el DIS block --> encontrar el siguiente DIS block next_dis()
    
    unsigned long int return_db_block_num;
    // Si quedan ptrs dentro del dis block, retorno el ptr al siguiente db
    if(file_desc->dis_block_offset < BLOCK_SIZE/4)
    {
        return curr_dis_block->pointers[file_desc->dis_block_offset];
    }
    // Si no quedan ptrs dentro del dis block, debo pasar al siguiente dis block
    else
    {
        file_desc->dis_blocks_read ++;
        // Tenemos que encontrar el siguiente DIS block
        file_desc->dis_block_offset = 0;
        file_desc->index_block_offset ++;

        // Si quedan ptrs a dis blocks en el curr index block
        if(file_desc->index_block_offset < curr_index_block->num_pointers)
        {
            // Se mantiene el index, avanzamos un dis y retornamos el primer db del dis
            Dis_block* new_dis_block = dis_block_init(curr_index_block->pointers[file_desc->index_block_offset]);
            memcpy(curr_dis_block, new_dis_block, sizeof(new_dis_block));
            free(new_dis_block);
            return_db_block_num = curr_dis_block->pointers[0];
            // printf("avance de dis\n");
            // printf("index offset: %i\n", file_desc->index_block_offset);
            // printf("dis offset: %i\n", file_desc->dis_block_offset);
            // printf("data offset: %i\n", file_desc->data_block_offset);
            return return_db_block_num;
        }
        else
        {
            // Avanzamos un index, actualizamos dis y db
            unsigned long int next_index_num = curr_index_block->next_index;
            Index_block* next_index_block = index_block_init(next_index_num, 0);
            memcpy(curr_index_block, next_index_block, sizeof(next_index_block));
            free(next_index_block);
            file_desc->index_block_offset = 0;
            file_desc->index_blocks_read ++;
            // printf("----------------------------\n");
            // printf("avance de index\n");
            // printf("index offset: %i\n", file_desc->index_block_offset);
            // printf("dis offset: %i\n", file_desc->dis_block_offset);
            // printf("data offset: %i\n", file_desc->data_block_offset);

            // Actualizamos el dis
            Dis_block* new_dis_block = dis_block_init(curr_index_block->pointers[0]);
            memcpy(curr_dis_block, new_dis_block, sizeof(new_dis_block));
            free(new_dis_block);
            return_db_block_num = curr_dis_block->pointers[0];
            return return_db_block_num;
        }
    }

}

int num_bloques_ocupados(unsigned char* buffer, long int buff_size)
{
    int num = 0;
    for(int i = 0; i < buff_size; i++) {
        int z = 128, oct = buffer[i];
        while (z > 0)
        {
            if (oct & z)
                num = num +1;
            z >>= 1;
        }
    }
    return num;
}