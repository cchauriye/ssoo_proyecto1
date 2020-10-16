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
    create_directory(parent_block_num, empty_block, empty_entry, dir_name);
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
    char *slash = path;
    char* dir_name;
    char* file_name;
    char* leftover;
    unsigned long int index_block_num = 0;
    unsigned long int dir_block_num = 0; // guardo el número del bloque que tiene la entrada
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        path = leftover;
        slash = leftover;
        dir_block_num = find_dir_entry_by_name(dir_block_num, dir_name);
        if(dir_block_num == -1){
            return 0;
        }
    };
    file_name = slash;
    index_block_num = find_dir_entry_by_name(dir_block_num, file_name); // Retorna bloque índice

    // Dejamos el bitmap del bloque indice en 0
    modify_bitmap(index_block_num, 0);
    
    ////// Debemos borrar la entrada de directorio
    
    
    return 0;
}
