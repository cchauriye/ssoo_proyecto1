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
        printf("Ruta no es válida");
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

    // Modo escritura --> creamos un archivo 
    if(mode == 'w') {
        unsigned long int index_block_num = find_block_by_path(path);
        
        // Revisamos que no existia el archivo
        if(index_block_num == -1){
            // Buscamos un bloque vacio en el bitmap
            unsigned long int empty_block = find_empty_block();

            // Buscamos el bloque dir donde va a ir este archivo
            unsigned long int parent_dir_block = find_parent_block_by_path(path);
            // Buscamos un empty dir entry en el directorio
            int empty_entry = find_empty_entry(parent_dir_block);
            // Sacamos el nombre del archivo
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
            // Setiamos el dir block entry
            create_dir_entry(parent_dir_block, empty_block, empty_entry, file_name, 1);

            // Setiamos el index block
            new_index_block(empty_block);
        }
    }

    // Ahora instanciamos el archivo que existia o que acabamos de crear
    osFile* file = malloc(sizeof(osFile));
    file->bytes_read = 0;
    file->index_blocks_read = 0;
    file->dis_blocks_read = 0;
    file->data_blocks_read = 0;
    file->index_block_offset = 0;
    file->dis_block_offset = 0;
    file->data_block_offset = 0;

    unsigned long int index_block_num = find_block_by_path(path);
    file->index_block_num = index_block_num;

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

/*int os_hardlink(char*orig, char*dest). Funcion que se encarga de crear un hardlink del archivo 
 referenciado  por origen  una  nueva  ruta dest,  aumentando  la  cantidad  de  referencias  
 al  archivo original.
*/
void os_hardlink(char*orig, char*dest){

    //1. Encontrar la entrada vacía del bloque padre del destino
    long unsigned int parent_block = find_parent_block_by_path(dest);
    int empty_entry = find_empty_entry(parent_block);
    unsigned char* dest_name = find_name_by_path(dest);
    printf("Name: %s\n", dest_name);

    //2. Encontrar el bloque ìndice del archivo origen
    unsigned int index_block_orig = find_block_by_path(orig);

    //3. Escribir en esa entrada que es un archivo y ponerle un puntero 
    // al bloque indice del origen y el nombre del archivo.
    int type = 1;
    write_entry_block(parent_block, index_block_orig, empty_entry, dest_name, type);

    //4. Escribir en el bloque ìndice de origen el hardlink al bloque destino.

    unsigned long int start = 2048*index_block_orig;
    int buff_size = 1;
    unsigned char buffer[buff_size];
    read_from_position(start, buffer, buff_size);

    int previous =  buffer[0];
    buffer[0] = buffer[0] + 1;

    FILE * pFile;
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 1, 1, pFile);
    fclose(pFile);

   // printf("bloque_padre: %i \n", parent_block);
    return;
}

void os_rmdir(char*path, bool recursive){
    unsigned long int block_num = find_block_by_path(path);
    printf("BLock_num: %i\n", block_num);
    if (block_num == -1)
    {
        printf("La ruta proporcionada no es válida, no se han encontrado directorios con ese nombre\n");
    }
    else
    {
    //Reviso si hay alguna entrada ocupada, si no hay borro el dir altiro, si hay reviso recursive
    Dir_block*  dir_block = dir_block_init(block_num);
    for (int i = 0; i < 64; i++)
    {
        printf("EStoy en el i: %i del for\n", i);
        Dir_block_entry* dir_entry = dir_block_entry_init(dir_block, i);
        if (dir_entry->valid) //significa que está ocupado
        {
            if (recursive){
            //recorro las entradas 
            //CASO 1: son archivos y los borro con os_rm
            //Armar el path para usar os_rm o usar parte de os_rm desde block_num
            // printf("BLOQUE: %i\n", block_num);
            // printf("path que entró al manejo path: %s\n", path);
            // printf("queremos eliminar: %s\n", dir_entry->name);
            // printf("EL valid es: %i\n",  dir_entry->valid);
            char entry_path[2000];
            strcpy(entry_path, path);
            char file_name[200] = "/";
            strcat(file_name, dir_entry->name);
            strcat(entry_path, file_name);
            printf("EL PATH HACIA EL ARCHIVO ES: %s\n", entry_path);

            //si es archivo, eliminamos
            if (dir_entry->valid == 1)
            {
                printf("Borré archivo %s\n",  dir_entry->name);
                os_rm(entry_path);
            }
            //si es directorio, aplicamos la funciòn recursivamente
            else if (dir_entry->valid == 2) 
            {
                printf("eliminando directorio %s\n", dir_entry->name);
                os_rmdir(entry_path, true);
            }
            
            
            
            
            //CASO 2: son directorios uso la funcion recursivamente  
            }
            else{
                printf("No ejecuta funcion porque hay archivos y es no recursivo\n");
                return;
            }
        // printf("La entrada %i del bloque %i está vacía\n", i, block_num);
        //free(dir_entry);
        // free(dir_block);
        } 
        free(dir_entry);
    }
    free(dir_block);
    //Si llego aca o estaba vacia o ya se borro todo lo de dentro
    //Para borrar directorio:
    //1- Libero el bitmap

    modify_bitmap(block_num,0);
    printf("Modifica el bitmap\n");
    //2- Libero la entrada del directorio padre los 2 bit= 00 ¿Es necesario puntero a 0 y nombre a 0? 
    unsigned int parent_block = find_parent_block_by_path(path);
    //unsigned char* name = find_name_by_path(path); //ACA SE CAE,la funcion funciona solo con cosas con extencion tipo ejemplo.txt Hay que arreglara!!
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
    printf("Dir name \n");
    dir_name = slash;
    printf("%s\n", dir_name);
    unsigned long int  num_entry = find_entry_num_by_name(parent_block, dir_name);
    unsigned long int start = 2048*parent_block +  32*num_entry;
    int value = 0;

    FILE * pFile;
    
    start = BLOCK_SIZE*parent_block + num_entry*32;
    unsigned char buffer[1];
    read_from_position(start, buffer, 1);
    buffer[0] = buffer[0] & 0b00111111;
    pFile = fopen(diskname, "r+");
    fseek(pFile, start, SEEK_SET);
    fwrite(buffer, 1, 1, pFile);
    fclose(pFile);
    }
}

int os_read(osFile* file_desc, void* buffer, int nbytes){
    // Revisar si nbytes es mayor que file_size - bytes_read
    if (nbytes > ((file_desc->index_block->file_size) - file_desc->bytes_read))
    {
        printf("Hemos leido %llu de %llu bytes\n", file_desc->bytes_read, file_desc->index_block->file_size);
        printf("Piden leer %i ahora\n", nbytes);
        nbytes = (file_desc->index_block->file_size) - file_desc->bytes_read;
        printf("Vamos a leer %i\n", nbytes);
    }

    // ------- Esta parte potencialmente se puede ahorrar si vamos actualizando los parametros de file_desc
    // // Tenemos que calcular desde donde comenzar leyendo
    // // En que bloque de datos quedo la ultima lectura
    // unsigned long int data_blocks_fully_read = file_desc->bytes_read / BLOCK_SIZE;
    // unsigned long int offset_in_last_data_block = file_desc->bytes_read % BLOCK_SIZE;

    // // Cuantos bloques DIS fueron leidos
    // unsigned long int dis_blocks_fully_read = data_blocks_fully_read*4 / BLOCK_SIZE;
    // unsigned long int offset_in_last_dis_block = (data_blocks_fully_read*4) % BLOCK_SIZE; // Numero del ultimo ptr a un bloque de datos que fue leido completo (si numeramos los ptrs en dis de 0 a 2048/4)

    // // Cuantos bloques indice fueron leidos
    // unsigned long int index_blocks_fully_read;
    // unsigned long int offset_in_last_index_block;
    // if (dis_blocks_fully_read*4 < 2036){
    //     index_blocks_fully_read = 0;
    //     offset_in_last_index_block = data_blocks_fully_read*4;
    // }
    // else{
    //     unsigned long int dis_blocks_read_in_first_index = 2036/4;
    //     index_blocks_fully_read = 1 + (dis_blocks_fully_read - dis_blocks_read_in_first_index)*4/2044;
    //     offset_in_last_index_block = (data_blocks_fully_read - dis_blocks_read_in_first_index)*4 % 2044;
    // }
    // --------------- Hasta aca ------------------

    // Funciones por hacer:

    // Procedimiento:
    // Siempre mantener index_block_read, dis_blocks_read, data_blocks_read
    // - Recorrer la lista ligada de Index_blocks hasta el index block que hay que leer
    // - Instanciar el DIS block que hay que leer
    // - Instanciar el DB que hay que leer
    // - Leer DB desde el offset
    // - Si se acaba, pasar al siguiente DB que apunte el DIS
    // - Si se acaba el DIS, pasar al siguiente DIS que apunte el index
    // - Si se acaba el index, pasar al siguiente index

    // Avanzamos hasta el index block correspondiente
    unsigned long int curr_index_num = file_desc->index_block_num;
    unsigned long int next_index_num = file_desc->index_block->next_index;
    Index_block* curr_index_block;
    if (file_desc->index_blocks_read == 0)
    {
        curr_index_block = index_block_init(curr_index_num, 1);
    }
    else
    {
        for (unsigned long int i = 0; i < file_desc->index_blocks_read; i++)
        {
            Index_block* next_index_block = index_block_init(next_index_num, 0);
            curr_index_num = next_index_num;
            next_index_num = next_index_block->next_index;
            free(next_index_block);
        }
        curr_index_block = index_block_init(curr_index_num, 0);
    }
    // Ya tenemos curr_index_block: bloque indice donde tenemos que leer

    // Instanciamos el DIS block que hay que leer
    Dis_block* curr_dis_block = dis_block_init(curr_index_block->pointers[file_desc->index_block_offset]);

    // Instanciamos el DB que hay que leer
    Data_block* curr_data_block = data_block_init(curr_dis_block->pointers[file_desc->dis_block_offset]);
    
    // read_data_block(): lee desde el offset del data block
    // Si lo que le queda del bloque por leer es menor a nbytes--> lee lo que queda. Retorna cuanto leyo.
    // Si lo que le queda del bloque por leer es mayor a nbytes --> lee los nbytes. Retorna 0.

    // Tomamos el numero retornado por read_data_block()
    // Si es mayor a 0 --> encontrar siguiente DB next_db()
    // next_db(file_desc, curr_dis, curr_index, dis_block_offset): retorna block num del siguiente DB
    // Si quedan ptrs por leer en el DIS block --> retornar block num del siguiente ptr
    // Si no quedan ptrs por leer en el DIS block --> encontrar el siguiente DIS block next_dis()

    // next_dis(file_desc, curr_index, index_block_offset)
    // Si quedan ptrs por leer en el index block --> retornar el block num del siguiente DIS
    // Si no quedan ptrs por leer en el index block --> retornar el block num del primer DIS en el siguiente Index

    int not_read_bytes = nbytes;
    int read_bytes;
    int total_read_bytes = 0;
    unsigned char small_buffer[2048];
    unsigned char buffer2[nbytes];
    while(not_read_bytes)
    {
        read_bytes = read_data_block(file_desc, curr_data_block, small_buffer, not_read_bytes, nbytes);
        not_read_bytes -= read_bytes;

        int buffer_offset = total_read_bytes;
        // Traspasar small buffer al buffer
        for (int i = 0; i <read_bytes; i++)
        {
            // memcpy(buffer2, small_buffer, read_bytes);
            buffer2[buffer_offset + i] = small_buffer[i];
        }
        total_read_bytes += read_bytes;
        // strcpy(buffer2, small_buffer);
        // buffer = &small_buffer;

        if(not_read_bytes)
        {
            unsigned long int next_db_num = next_db(file_desc, curr_index_block, curr_dis_block, curr_data_block);
            Data_block* new_data_block = data_block_init(next_db_num);
            memcpy(curr_data_block, new_data_block, sizeof(new_data_block));
            free(new_data_block);
        }   
    }    
    
    free(curr_data_block);
    free(curr_dis_block);
    free(curr_index_block);

    // for (int i = 0; i < total_read_bytes; i++)
    // {
    //     printf("%c", buffer2[i]);
    // }
    
    memcpy(buffer,(unsigned char *)buffer2, total_read_bytes);
    return total_read_bytes;
}

int os_write(osFile* file_desc, void* buffer, int nbytes){
    
    unsigned char buffer2[nbytes]; 
    memcpy(buffer2,(unsigned char *)buffer, nbytes); // Falta caso borde
    file_desc -> index_block -> file_size = nbytes; // Falta caso borde
    
    // printf("1. filesize: %llu\n", file_desc -> index_block -> file_size);
    // save_index_block(file_desc->index_block);
    // Index_block* temp = index_block_init(file_desc->index_block_num, 1);
    // printf("2. filesize: %llu\n", temp->file_size);
    // free(temp);

    // El bloque es nuevo (index vacio)
    if(file_desc -> index_block -> file_size == nbytes)
    {
        // Calculamos la cantidad de bloques que utilizará el archivo
        unsigned long int db_used = nbytes / BLOCK_SIZE;
        file_desc -> data_blocks_used = db_used;
        file_desc -> data_block_offset = nbytes % BLOCK_SIZE;
        if(nbytes % BLOCK_SIZE)
        {
            db_used++;
        }

        // Creamos un maxi array que contiene todos los punteros a db que usaremos
        unsigned long int maxito[db_used];

        file_desc -> indirect_blocks_used = db_used / 512;
        file_desc -> dis_block_offset = db_used % 512;
        if(db_used % 512)
        {
            db_used++;
        }

        file_desc -> index_blocks_used = 1;
        if(file_desc -> indirect_blocks_used <= 509)
        {
            file_desc -> index_block_offset = file_desc -> indirect_blocks_used % 509;
        }
        else
        {
            file_desc -> index_blocks_used += (file_desc -> indirect_blocks_used - 509) / 511;
            unsigned long int indies = file_desc -> indirect_blocks_used - 509 % 511;
            if(indies)
            {
                file_desc -> index_blocks_used ++;
            }
            file_desc -> index_block_offset = file_desc -> indirect_blocks_used % 511;
        }
        // Creamos el resto de los index block

        unsigned long int empty_block;
        empty_block = find_empty_block();

        for(unsigned long int i = 0; i < file_desc -> index_blocks_used - 1; i++)
        {
            modify_bitmap(empty_block, 1);
            if(i==0)
            {
                file_desc -> index_block -> next_index = empty_block;
            }
            else
            {
                Index_block* new_index_block = index_block_init(empty_block, 0);
                empty_block = find_empty_block();
                new_index_block -> next_index = empty_block;
                save_index_block(new_index_block);
                free(new_index_block);
            }
        }

        Index_block* curr_index_block = index_block_init(file_desc->index_block_num, 1);

        empty_block = find_empty_block();
        unsigned long int next_index;
        next_index = curr_index_block -> next_index;
        unsigned long int counter = 0;

        for(unsigned long int i = 0; i < file_desc -> index_blocks_used; i++)
        {
            for(unsigned long int j = 0; j < curr_index_block ->num_pointers; j++)
            {
                curr_index_block -> pointers[j] = empty_block;
                modify_bitmap(empty_block, 1);
                empty_block = find_empty_block();
                
                Dis_block* new_dis_block = dis_block_init(empty_block);
                for(unsigned long int k = 0; k < 512; k++)
                {
                    new_dis_block -> pointers[k] = empty_block;
                    modify_bitmap(empty_block, 1);
                    maxito[counter] = empty_block;
                    counter++;
                    empty_block = find_empty_block();
                }
                save_dis_block(new_dis_block);
                free(new_dis_block);
            }
            Index_block* next_index_block = index_block_init(next_index, 0);
            next_index = next_index_block -> next_index;
            memcpy(curr_index_block, next_index_block, sizeof(next_index_block));
            save_index_block(next_index_block);
            free(next_index_block);
        }
        save_index_block(curr_index_block);
        free(curr_index_block);

        Index_block* first_index_block = index_block_init(file_desc->index_block_num, 1);
        printf("filesize: %llu\n", first_index_block->file_size);
        free(first_index_block);

        // Ahora que tenemos a maxito podemos llegar y escribir
        FILE * pFile;
        pFile = fopen(diskname, "r+");
        unsigned long int start;
        unsigned char small_buffer[2048];

        for(unsigned long int m = 0; m < db_used; m++)
        {
            start = m*2048;
            fseek(pFile, start, SEEK_SET);
            for(unsigned long int s = 0; s < BLOCK_SIZE; s++)
            {
                small_buffer[s] = buffer2[BLOCK_SIZE*m + s];
            }

            if(m < db_used - 1)
            {
                fwrite(small_buffer, BLOCK_SIZE, 1, pFile);
            }
            else
            {
                fwrite(small_buffer, file_desc -> data_block_offset, 1, pFile);
            }
            fclose(pFile);
        }
    }

    // Hacer función para verificar que el disco se llenó

    // Hay que agregar la referencia al nuevo index block que potencialmente se creó en next_db. 

    // Al final tenemos que actualizar el index_block -> file_size

    // Retornamos la cantidad de bytes efectivamente escritos.
}

// Retorna 0 si sale todo bien, 1 en otro caso
int os_unload(char* orig, char* dest){

    // Verificamos que el path existe
    unsigned long int index_block_num = find_block_by_path(orig);
    if(index_block_num == -1)
    {
        return 1;
    }

    // Leemos el archivo
    osFile* f = os_open(orig, 'r');
    unsigned long long int bytes_to_read = f->index_block->file_size;
    printf("File size: %llu\n", bytes_to_read);

    // Vamos leyendo el archive de a buff_size bytes
    int buff_size = 10000;
    unsigned char buffer[buff_size];
    int bytes_read;
    unsigned long long int total_bytes_read = 0;

    // Creamos el archivo nuevo
    FILE *fp;
    fp = fopen(dest, "wb");

    while (bytes_to_read)
    {
        bytes_read = os_read(f, buffer, buff_size);
        bytes_to_read -= bytes_read;
        // printf("bytes_read = %i\n", bytes_read);
        // printf("bytes_to_read = %i\n", bytes_to_read);
        // for (int i = 0; i < bytes_read; i++)
        // {
        //     printf("%c", buffer[i]);
        // }
        // printf("\n");

        // fseek(fp, total_bytes_read, SEEK_SET);
        fwrite(buffer, bytes_read, 1 , fp);
        total_bytes_read += bytes_read;
    }

    fclose(fp);
    os_close(f);
    return 0;
}