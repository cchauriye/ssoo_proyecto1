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

    os_exists("/folder/god/shrek.mp4");
    os_exists("/folder/god/shrekii.mp4");
    
    // Probar os_mkdir
    //os_bitmap(0, false);
    // os_mkdir("/folder/god/chaurri5");
    os_ls("/folder/god");
    os_ls("/folder/godSDF");
    // os_bitmap(1, false);

    // Probar os_rm
    // os_ls("/");
    // unsigned long int parent_block = find_parent_block_by_path("/IMPORTANT.txt");
    // printf("[main] parent block: %lu\n", parent_block);
    // unsigned long int index_block = find_block_by_path("/IMPORTANT.txt");
    // printf("[main] index block num: %lu\n", index_block);
    // Index_block* index = index_block_init(index_block, 1);
    // printf("hardlinks: %i\n", index->num_hardlinks);
    // free(index);
    // printf("-----OS RM--------\n");
    //os_rm("/IMPORTANT.txt");
    // os_ls("/");
    // Index_block* index2 = index_block_init(index_block, 1);
    // printf("hardlinks: %i\n", index2->num_hardlinks);
    // free(index2);

    // Probar os_open

    // os_bitmap(1, false);

    osFile* f = os_open("/folder/go0d/mora22.txt", 'r');
    //osFile* f2 = os_open("/folder/god/mora13.txt", 'w');
    //osFile* f3 = os_open("/folder/god/mora16.txt", 'w');

    // printf("index block \n hardlinks: %i \n position: %u\n file size: %llu\n", f->index_block->num_hardlinks, f->index_block->position, f->index_block->file_size);
    // printf("f: \nindex blocks used: %lu\n indirect blocks used: %lu\n data blocks used: %lu\n", f->index_blocks_used, f->indirect_blocks_used, f->data_blocks_used);
    // int closed = os_close(f);
    //os_bitmap(1, false);

    
    //print_all_entries_from_dir(536679);
    printf("\n");
    //print_all_entries_from_dir(285598);
    // os_rm("/folder/god/mora17.txt");
    os_rm("/folder/god/mora16.txt");
    print_all_entries_from_dir(536679);
    printf("\n");
    // os_mkdir("/folder/god/ejemplo8");
    // os_mkdir("/folder/god/ejemplo8");
    // print_all_entries_from_dir(536679);
    print_all_entries_from_dir(285598);
    //print_all_entries_from_dir(285598);

    //os_hardlink("/folder/god/shrek.mp4", "/folder/nuevo.mp4");
    //print_all_entries_from_dir(285598);
    //os_mkdir("/folder/god/ejem8/ejemplito9");
    printf("\n");
    //print_all_entries_from_dir(285598);
    // os_rmdir("/folder/god", true);
    //print_all_entries_from_dir(285598);
    // printf("\n");

    //print_all_entries_from_dir(536679);
    printf("\n");
    //print_all_entries_from_dir(285598);
    

    // os_bitmap(1, false);

    // Probar os_ls
    // os_ls("/folder/god");


    // Probar os_read
    // osFile* f = os_open("/folder/napkin.txt", 'r');
    // int nbytes = 10000;
    // unsigned char buffer[nbytes];
    // int bytes_read = os_read(f, buffer, nbytes);
    // printf("bytes_read = %i\n", bytes_read);
    // printf("buffer: %s\n", buffer);
    // os_close(f);

    return 0;
}