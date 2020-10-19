#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h>
#include "API/os_API.h"
#include "blocks/dir_block.h"
#include <math.h>
#include "functions/functions.h"

extern char* diskname;

int main(int argc, char *argv[])
{
    // ------------- MAIN 1 ------------- //
    os_mount(argv[1]);

    // ------------- MAIN 2: os_bitmap ------------- //
    // Todo el bitmap en Hexadecimal
    // os_bitmap(0, true);
    // Todo el bitmap en Binario
    // os_bitmap(0, false);
    // Un bitmap en Hex
    // os_bitmap(1, true);
    // Un bitmap en Bin
    // os_bitmap(10, false);
    // Un bitmap que no existe
    // os_bitmap(66, false);
    
// ------------- MAIN 3: os_exists ------------- //
    // // Probar os_exists
    // int root = os_exists("/");
    // int not_existing = os_exists("/folder/yo/no_existo.rar");
    // int exist = os_exists("/folder/god/shrek.mp4");
    // fprintf(stderr, "root: %i\nnot existing: %i\nexists: %i\n", root, not_existing, exist);

// ------------- MAIN 4: os_ls ------------- //
    // Explicar que partimos los path con "/"
    // os_ls("/");
    // os_ls("/folder/god");
    // os_ls("/IMPORTANT.txt");
    // os_ls("/folder/nada");

// ------------- MAIN 4: os_open ------------- //
    // Modo read archivo que si existe
    // osFile* f = os_open("/folder/napkin.txt", 'r');
    // fprintf(stderr, "index block \nhardlinks: %i\n position: %u\n file size: %llu\n", f->index_block->num_hardlinks, f->index_block->position, f->index_block->file_size);
    // fprintf(stderr, "f:\nindex blocks used: %lu\nindirect blocks used: %lu\ndata blocks used: %lu\n", f->index_blocks_used, f->indirect_blocks_used, f->data_blocks_used);
    // os_close(f);
    // Modo read archivo que no existe
    // osFile* f = os_open("/folder/NOTnapkin.txt", 'r');
    // Modo write
    // os_bitmap(1, false);
    // osFile* f2 = os_open("/folder/nuevo2.txt", 'w');
    // fprintf(stderr, "index block \nhardlinks: %i\nposition: %u\nfile size: %llu\n", f2->index_block->num_hardlinks, f2->index_block->position, f2->index_block->file_size);
    // fprintf(stderr, "f: \nindex blocks used: %lu\nindirect blocks used: %lu\ndata blocks used: %lu\n", f2->index_blocks_used, f2->indirect_blocks_used, f2->data_blocks_used);
    // os_close(f2);
    // os_bitmap(1, false);
    // os_ls("/folder");

// ------------- MAIN 5: os_read ------------- //
     // Probar os_read caso 1: Leer 2 veces
    // osFile* f = os_open("/folder/napkin.txt", 'r');
    // int nbytes = 100;
    // unsigned char buffer[nbytes];
    // int bytes_read = os_read(f, buffer, nbytes);
    // fprintf(stderr, "bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     fprintf(stderr, "%c", buffer[i]);
    // }
    // fprintf(stderr, "\nSegundo read\n");

    // nbytes = 900;
    // unsigned char buffer2[nbytes];
    // bytes_read = os_read(f, buffer2, nbytes);
    // fprintf(stderr, "bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     fprintf(stderr, "%c", buffer2[i]);
    // }
    // os_close(f);

    // Probar os_read caso 2: Leer mas del file size
    // fprintf(stderr, "\n\n");
    // osFile* f = os_open("/IMPORTANT.txt", 'r');
    // fprintf(stderr, "File size: %llu\n", f->index_block->file_size);
    // int nbytes = 55000;
    // unsigned char buffer2[nbytes];
    // int bytes_read = os_read(f, buffer2, nbytes);
    // fprintf(stderr, "\n---------------First read--------------------\n");
    // fprintf(stderr, "Bytes_read = %i\n", bytes_read);

    // nbytes = 1000;
    // unsigned char buffer3[nbytes];
    // bytes_read = os_read(f, buffer3, nbytes);
    // fprintf(stderr, "---------------Second read--------------------\n");
    // fprintf(stderr, "Bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     fprintf(stderr, "%c", buffer3[i]);
    // }
    // os_close(f);
    // fprintf(stderr, "\n\n");

    // ------------- MAIN 6: os_unload ------------- //
    // Probar os_unload caso 1
    // os_unload("/folder/napkin.txt", "nueva_napkin.txt");
    // Probar os_unload caso 2
    // os_unload("/IMPORTANT.txt", "nueva_IMPORTANT2.txt");

        // ------------- MAIN 9: os_hardlinks ------------- //
    // Caso 1: Crear un Hardlink
    // os_hardlink("/folder/god/shrek.mp4", "/folder/nuevo_shrek.mp4");
    // unsigned long int block_num = find_block_by_path("/folder/god/shrek.mp4");
    // fprintf(stderr, "Index block: %lu\n", block_num);
    // block_num = find_block_by_path("/folder/nuevo_shrek.mp4");
    // fprintf(stderr, "Index block: %lu\n", block_num);
    // block_num = find_block_by_path("/folder");
    // print_all_entries_from_dir(block_num);

    // Caso 2: destino no valido
    // os_hardlink("/folder/god/shrek.mp4", "/NOTfolder/nuevo_shrek.mp4");

    // ------------- MAIN 7: os_mkdir ------------- //
    // Caso 1: Crear un directorio
    // unsigned long int block_num = find_block_by_path("/folder/god");
    // print_all_entries_from_dir(block_num);
    // printf("\n\n");
    // os_mkdir("/folder/god/nuevo_dir2");
    // print_all_entries_from_dir(block_num);

    // Caso 2: Crear un directorio en una ruta invalida
    // os_mkdir("/folder/NOTgod/nuevo_dir2");

    // Caso 3: Crear un directorio que ya existe
    // unsigned long int block_num = find_block_by_path("/folder/god");
    // print_all_entries_from_dir(block_num);
    // printf("\n\n");
    // os_mkdir("/folder/god/nuevo_dir2");
    // print_all_entries_from_dir(block_num);

    // Caso 4: Intentar crear un directorio en un bloque directorio lleno
    // char path[100] = "/folder/god/nuevo_dir";
    // char entry_path[100];
    // for(int i=0; i<64; i++)
    // {
    //     sprintf(path, "/folder/god/nuevo_dir%i", i);
    //     os_mkdir(path);
    // }
    // unsigned long int block_num = find_block_by_path("/folder/god");
    // printf("\n\n");
    // print_all_entries_from_dir(block_num);

    // ------------- MAIN 8: os_rmdir ------------- //
    // Caso 1: Remover un directorio //
    // os_rmdir("folder/god", true);
    // unsigned long int block_num = find_block_by_path("/folder");
    // printf("\n\n");
    // print_all_entries_from_dir(block_num);
    // printf("\n\n");
    // block_num = find_block_by_path("/folder/god");
    // print_all_entries_from_dir(block_num);


    // Caso 2: Remover un directorio que no existe
    // os_rmdir("folder/god", false);
    // unsigned long int block_num = find_block_by_path("/folder");
    // printf("\n\n");
    // print_all_entries_from_dir(block_num);

    // Caso 3: Remover un directorio no recursivo
    // os_rmdir("/music", false);
    // unsigned long int block_num = find_block_by_path("/music");
    // printf("\n\n");
    // print_all_entries_from_dir(block_num);
    // block_num = find_block_by_path("/music");
    // print_all_entries_from_dir(block_num);

    // ------------- MAIN 9: os_rm ------------- //
    
    return 0;
}