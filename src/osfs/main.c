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
    
    os_mount(argv[1]);   


    // Probar os_bitmap
    // Todo el bitmap en Hexadecimal
    // os_bitmap(0, true);
    // Todo el bitmap en Binario
    // os_bitmap(0, false);
    // Un bitmap en Hex
    // os_bitmap(1, true);
    // Un bitmap en Bin
    // os_bitmap(10, false);
    

    // Probar os_exists
    // int root = os_exists("/");
    // int not_existing = os_exists("/folder/yo/no_existo.rar");
    // int exist = os_exists("/folder/god/shrek.mp4");
    // printf("%i\n%i\n%i\n", root, not_existing, exist);

    // Probar os_ls
    // os_ls("/");
    // os_ls("/folder");

    // Probar os_open
    // Modo read
    // osFile* f = os_open("/folder/napkin.txt", 'r');
    // printf("index block \n hardlinks: %i \n position: %u\n file size: %llu\n", f->index_block->num_hardlinks, f->index_block->position, f->index_block->file_size);
    // printf("f: \nindex blocks used: %lu\n indirect blocks used: %lu\n data blocks used: %lu\n", f->index_blocks_used, f->indirect_blocks_used, f->data_blocks_used);
    // os_close(f);
    // Modo write
    // os_bitmap(1, false);
    // osFile* f2 = os_open("/folder/nuevo.txt", 'w');
    // printf("index block \n hardlinks: %i \n position: %u\n file size: %llu\n", f2->index_block->num_hardlinks, f2->index_block->position, f2->index_block->file_size);
    // printf("f: \nindex blocks used: %lu\n indirect blocks used: %lu\n data blocks used: %lu\n", f2->index_blocks_used, f2->indirect_blocks_used, f2->data_blocks_used);
    // os_close(f2);
    // os_bitmap(1, false);

     // Probar os_read caso 1
    // osFile* f = os_open("/folder/napkin.txt", 'r');
    // int nbytes = 100;
    // unsigned char buffer[nbytes];
    // int bytes_read = os_read(f, buffer, nbytes);
    // printf("bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     printf("%c", buffer[i]);
    // }
    // printf("\nSegundo read\n");

    // nbytes = 900;
    // unsigned char buffer2[nbytes];
    // bytes_read = os_read(f, buffer2, nbytes);
    // printf("bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     printf("%c", buffer2[i]);
    // }
    // os_close(f);

    // Probar os_read caso 2
    // osFile* f = os_open("/folder/god/shrek.mp4", 'r');
    // printf("file size: %llu\n", f->index_block->file_size);
    // int nbytes = 100000;
    // unsigned char buffer[nbytes];
    // int bytes_read = os_read(f, buffer, nbytes);
    // printf("bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     printf("%c", buffer[i]);
    // }
    // os_close(f);

    // Probar os_read caso 3
    // osFile* f = os_open("/IMPORTANT.txt", 'r');
    // printf("file size: %llu\n", f->index_block->file_size);
    // int nbytes = 30;
    // unsigned char buffer[nbytes];
    // int bytes_read = os_read(f, buffer, nbytes);
    // printf("bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     printf("%c", buffer[i]);
    // }

    // nbytes = 2000;
    // unsigned char buffer2[nbytes];
    // bytes_read = os_read(f, buffer2, nbytes);
    // printf("\n---------------Second read--------------------\n");
    // printf("bytes_read = %i\n", bytes_read);

    // nbytes = 1000;
    // unsigned char buffer3[nbytes];
    // bytes_read = os_read(f, buffer3, nbytes);
    // printf("---------------Third read--------------------\n");
    // printf("bytes_read = %i\n", bytes_read);
    // for (int i = 0; i < bytes_read; i++)
    // {
    //     printf("%c", buffer3[i]);
    // }
    // os_close(f);

    // Probar os_rm
    // os_ls("/");
    // unsigned long int index_block = find_block_by_path("/IMPORTANT.txt");
    // Index_block* index = index_block_init(index_block, 1);
    // printf("hardlinks: %i\n", index->num_hardlinks);
    // free(index);
    // os_rm("/IMPORTANT.txt");
    // os_ls("/");
    // Index_block* index2 = index_block_init(index_block, 1);
    // printf("hardlinks: %i\n", index2->num_hardlinks);
    // free(index2);
    
    // Probar os_mkdir
    // os_bitmap(1, false);
    // os_mkdir("/folder/god/caso5");
    // os_ls("/folder/god");
    // os_bitmap(1, false);


    //os_hardlink("/folder/god/shrek.mp4", "/foldder/nuevo.mp4");
    //print_all_entries_from_dir(536679);
    //print_all_entries_from_dir(285598);
    //os_mkdir("/folder/god/ejem8/ejemplito9");
    // printf("\n");
    // //print_all_entries_from_dir(285598);
    // os_rmdir("/folder/god", true);
    // print_all_entries_from_dir(285598);
    // // printf("\n");

    //print_all_entries_from_dir(536679);
    // printf("\n");

    

    // os_bitmap(1, false);


   
    // Probar os_unload caso 1
    // os_unload("/folder/napkin.txt", "nueva_napkin.txt");

    // Probar os_unload caso 2
    // os_unload("/IMPORTANT.txt", "nueva_IMPORTANT2.txt");

    // Probar os_write
    // osFile* f = os_open("/folder/keso1.txt", 'w');
    // unsigned char* buffer = "Hola como estas biuqvd qd q  wcqwl";
    // os_write(f, buffer, 10);
    // os_close(f);
    // os_ls("/folder");
    // os_unload("/folder/keso.txt", "local_keso.txt");

    return 0;
}