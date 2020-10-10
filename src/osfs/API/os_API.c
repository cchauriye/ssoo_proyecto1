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

// Imprimir bitmap
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

// Imprimir nombres de archivos en el path dado
void os_ls(char* path){

}

// Split del path
char* dir_name_from_path(char* path){
    char *slash = path;
    char* dir_name;
    char* leftover;
    while(strpbrk(slash+1, "\\/")){
        slash = strpbrk(slash+1, "\\/");
        dir_name = strndup(path, slash - path);
        leftover = strdup(slash+1);
        printf("%s\n", dir_name);
        path = leftover;
        slash = leftover;
    };
    dir_name = slash;
    printf("%s\n", dir_name);
    return dir_name;
}