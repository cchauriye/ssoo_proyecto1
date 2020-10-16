#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h>
#include "os_API.h"
#include "../functions/functions.h"

char* diskname;
unsigned int BLOCK_SIZE = 2048;

// Establece como variable global la ruta local del disco
void os_mount(char* disk){
    diskname = disk;
}

// Imprimir bitmap  AL FINAL ARREGLAR A LA CANTIDAD DE BYTES CORRECTA
void os_bitmap(unsigned num, bool hex){
    long int buff_size = 15;
    unsigned char buffer[buff_size];
    if (num == 0){
        for (int b = 1; b < 65; b++)
        {
            read_from_position(BLOCK_SIZE*b, buffer, buff_size);
            if(hex){
                unsigned char hex_buffer[buff_size * 2];
                print_hex_buffer(hex_buffer, buffer, buff_size);
                for (int i=0; i<buff_size*2; i++){
                    printf("%c", hex_buffer[i]);
                }
                printf("\n");
            }
            else {
                print_binary_buffer(buffer, buff_size);
            }
        }
        
    }
    else {
        read_from_position(BLOCK_SIZE*num, buffer, buff_size);
        if(hex){
            unsigned char hex_buffer[buff_size * 2];
            print_hex_buffer(hex_buffer, buffer, buff_size);
            for (int i=0; i<buff_size*2; i++){
                printf("%c", hex_buffer[i]);
            }
            printf("\n");
        }
        else {
            print_binary_buffer(buffer, buff_size);
        }
    }
}

// Verificar si archivo existe.
int os_exists(char* path){
    if (find_block_by_path(path) == -1)
    {
        return 0;
    }
    return 1;
}

void os_ls(char* path){
    long unsigned int dir_block_num = find_block_by_path(path);
    if (dir_block_num == -1){
        return;
    }
    else{
        print_files_from_dir(dir_block_num);
    }
}

//crear un direcotrio con el path dado
int os_mkdir(char* path){  
    // Buscamos un bloque vacio en el bitmap
    unsigned long int empty_block = find_empty_block();
    // Buscamos el bloque directorio donde tenemos que crear la entrada
    unsigned long int parent_block_num = find_parent_block_by_path(path);
    if (parent_block_num == -1) {
        return 1;
    }

    char *slash = path;
    char* dir_name;
    char* file_name;
    char* leftover;
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        printf("leftover: %s\n", leftover);
        path = leftover;
        slash = leftover;
    };
    dir_name = slash;
    printf("dir name: %s\n", dir_name);

    // Buscamos una entrada vacia
    int empty_entry = find_empty_entry(parent_block_num);

    // Creamos el directorio
    create_dir_entry(parent_block_num, empty_block, empty_entry, dir_name, 2);
    return 0;
}

osFile* os_open(char* path, char mode) {
    
    osFile* file = malloc(sizeof(osFile));
    // Modo lectura
    if(mode == 'r') {
        unsigned long int index_block_num = find_block_by_path(path);
        printf("El index esta en el block num: %lu\n", index_block_num);

        // Instanciamos el Index Block
        Index_block* index_block = index_block_init(index_block_num, 1);

        file ->index_block = index_block;

        // Calculamos cuantos data blocks usa
        unsigned long int data_blocks_used = (index_block->file_size / BLOCK_SIZE);
        if(index_block->file_size % BLOCK_SIZE) {
            data_blocks_used += 1;
        }
        file->data_blocks_used = data_blocks_used;

        // Claculamos cuantos Bloques de direccionamiento simple usa
        unsigned long int bytes_used_for_pointers_to_data = (data_blocks_used * 4);
        unsigned long int indirect_blocks_used = (bytes_used_for_pointers_to_data / BLOCK_SIZE);
        if(bytes_used_for_pointers_to_data % BLOCK_SIZE) {
            indirect_blocks_used += 1;
        }
        file->indirect_blocks_used = indirect_blocks_used;

        // Calculamos cuantos bloques indice usa
        unsigned long int index_blocks_used;
        unsigned long int bytes_used_for_pointers_to_indirect = (indirect_blocks_used * 4);
        if (bytes_used_for_pointers_to_indirect <= 2036){
            index_blocks_used = 1;
        }
        else{
            index_blocks_used = 1;
            bytes_used_for_pointers_to_indirect -= 2036;
            index_blocks_used += bytes_used_for_pointers_to_indirect / 2044;
            if(bytes_used_for_pointers_to_indirect % 2044) {
                index_blocks_used += 1;
            }
        }
        file->index_blocks_used = index_blocks_used;
    }

    // Modo escritura
    if(mode == 'w') {
        unsigned long int index_block_num = find_block_by_path(path);
        
        // Revisamos que no existia el archivo
        if(index_block_num == -1){
            // Buscamos un bloque vacio en el bitmap y lo marcamos como ocupado
            unsigned long int empty_block = find_empty_block();

            // Buscamos el bloque dir donde va a ir este archivo
            unsigned long int parent_dir_block = find_parent_block_by_path(path);
            // Buscamos un empty dir entry en el directorio
            int empty_entry = find_empty_entry(parent_dir_block);
            // Sacamos el nombre del archivo
            char *slash = path;
            char* dir_name;
            char* file_name;
            char* leftover;
            while(strpbrk(slash+1, "\\/")){
                slash = strpbrk(slash+1, "\\/");
                dir_name = strndup(path, slash - path);
                leftover = strdup(slash+1);
                path = leftover;
                slash = leftover;
            };
            file_name = slash;
            printf("file name: %s\n", file_name);
            // Setiamos el bloque indice
            create_dir_entry(parent_dir_block, empty_block, empty_entry, file_name, 1);


        }

    }
    return file;
}

int os_close(osFile* file_desc){
    free(file_desc->index_block);
    free(file_desc);
    return 0;
}

int os_rm(char* path){
    char path2[100];
    strcpy(path2, path);
    // Extract the first token
    char* next_dir = strtok(path2, "/");
    char* prev_dir;
    // loop through the string to extract all other tokens
    while( next_dir != NULL) {
        prev_dir = next_dir;
        next_dir = strtok(NULL, "/");
    }
    char* file_name = prev_dir;
    
    // Restamos 1 a los Hard Links
    unsigned long int index_block_num = find_block_by_path(path);
    unsigned long int start = BLOCK_SIZE*index_block_num;
    unsigned char buffer[1];
    read_from_position(start, buffer, 1);
    int num_of_hl = buffer[0];
    num_of_hl --;
    buffer[0] = num_of_hl;
    FILE * pFile;
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 1, 1, pFile);
    fclose(pFile);

    // Vamos a modificar valid en dir entry
    unsigned long int parent_block = find_parent_block_by_path(path);
    int entry_block_num = find_entry_num_by_name(parent_block, file_name);
    
    start = BLOCK_SIZE*parent_block + entry_block_num*32;
    read_from_position(start, buffer, 1);
    print_binary_buffer(buffer, 1);
    buffer[0] = buffer[0] & 0b00111111;
    print_binary_buffer(buffer, 1);
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 1, 1, pFile);
    fclose(pFile);

     // Dejamos el bitmap del bloque indice en 0
    modify_bitmap(entry_block_num, 0);

    return 0;
}
