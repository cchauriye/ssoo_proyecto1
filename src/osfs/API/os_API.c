#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h>
#include "os_API.h"

char* diskname;
unsigned int BLOCK_SIZE = 2048;

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

// Establece como variable global la ruta local del disco
void os_mount(char* disk){
    diskname = disk;
}

// Crear archivo binario de prueba
void create_test_bin(){
    FILE *fp;
    unsigned char mibyte[8] = "10101010";
    fp = fopen( "test.bin" , "wb" );
    fwrite(mibyte, sizeof(mibyte), 1 , fp);
    fclose(fp);
}

// Imprimir bitmap  AL FINAL ARREGLAR A LA CANTIDAD DE BYTES CORRECTA
void os_bitmap(unsigned num, bool hex){
    long int buff_size = 20;
    unsigned char buffer[buff_size];
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

// Verificar si archivo existe.
int os_exists(char* path){
    struct stat s;
    int err = stat(path, &s);
    if(-1 == err)
    {
        printf("File does not exist.\n");
        return 0;
    }
    return 1;
}

// Imprimir nombres de archivos en el path dado haciendo split del path
void os_ls(char* path){
    char *slash = path;
    char* dir_name;
    char* leftover;
    int block_num = 0;
    int counter = 1;
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        printf("Dir %i: %s\n", counter, dir_name);
        path = leftover;
        slash = leftover;
        block_num = find_dir_entry_by_name(block_num, dir_name);
        if(block_num == -1){
            return;
        }
        counter++;
    };
    dir_name = slash;
    block_num = find_dir_entry_by_name(block_num, dir_name);
    printf("sali while hehe Dir %i: %s\n", counter, dir_name);
    print_files_from_dir(block_num, dir_name);
    return;
}


//crear un direcotrio con el path dado
void os_mkdir(char* path){

    //Tenemos que encontrar un bloque vacío en el bitmap
    int founded = 0;
    int empty_block;
    long int buff_size = BLOCK_SIZE;
    unsigned char buffer[buff_size];
    int num = 1;
    while (num < 2048 && founded==0)
    {
        read_from_position(BLOCK_SIZE*num, buffer, buff_size); 
        int i = 0;
        while(i < buff_size && founded==0) {
            int bit = 0;
            int z = 128, oct = buffer[i];
            int nada;
            while (z > 0)
            {
                if (oct & z){
                    nada = 10;
                }
                else{
                    printf("Bloque vacío! Bloque de bitmap = %i, BYTE: %i, (valor de z: %i, bit: %i)\n", num, i, z, bit);
                    // Transformar num, i y z en el numero de bloque vacio
                    empty_block = 2048*(num-1)*8 + 8*(i) + bit;
                    printf("Equivale al bloque numero: %i\n", empty_block);
                    founded = 1;
                    break;
                }
                z >>= 1;
                bit++;
            }
        fprintf(stderr, "\n");
        i++;
        }     
    num ++;  
    }
  

    char *slash = path;
    char* dir_name;
    char* leftover;
    int block_num;
    int counter = 1;

    //iteramos en el path
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        printf("Dir %i: %s\n", counter, dir_name);
        path = leftover;
        slash = leftover;
        block_num = find_dir_entry_by_name(block_num, dir_name);
        // if(block_num == -1){
        //     return;
        // }
        counter++;
    };
    printf("Blocknum: %i\n", block_num);
    /*
    Tenemos el directorio anterior (blocknum), debemos 
    buscar una entrada vacía para asignarle la dirección de memoria
    del bloque vacío que encontramos arriba
    */
    int parent_block = block_num;
    int empty_entry = find_empty_entry(parent_block);
    //printf("entrada: %i\n", empty_entry);
    
    dir_name = slash; //en caracter 
  
    printf("Dir name: %s\n", dir_name);
    printf("Empty block: %i\n", empty_block);

    create_directory(parent_block, empty_block, empty_entry, dir_name);
    //char* binary_name = stringToBinary(dir_name);
    //printf("nombre en binario: %s\n", binary_name);

    // if (empty_entry){
    //     create_directory(empty_block, parent_block, empty_entry, binary_name);
    // }
    // else{
    //     printf("No hay más espacio para almacenar dentro de esta carpeta");
    // }


    /*
    dir_name = slash;
    Tenemos el nombre del directorio que queremos crear,
    es dir_name. Basta con asignárselo a este nuevo bloque directorio
    según cómo se inserta la info.
    */

   

    dir_name = slash;
    block_num = find_dir_entry_by_name(block_num, dir_name);
    printf("Dir %i: %s\n", counter, dir_name);
    print_files_from_dir(block_num, dir_name);
    return;

    
}