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

    // PROBAR MKDIR
    // os_bitmap(1, false);
    // os_mkdir("/folder/god/chaurri5");
    // os_ls("/folder/god");
    // os_bitmap(1, false);

    // PROBAR os_rm
    os_ls("/");
    // unsigned long int parent_block = find_parent_block_by_path("/IMPORTANT.txt");
    // printf("[main] parent block: %lu\n", parent_block);
    unsigned long int index_block = find_block_by_path("/IMPORTANT.txt");
    printf("[main] index block num: %lu\n", index_block);
    Index_block* index = index_block_init(index_block, 1);
    printf("hardlinks: %i\n", index->num_hardlinks);
    free(index);
    printf("-----OS RM--------\n");
    os_rm("/IMPORTANT.txt");
    os_ls("/");
    Index_block* index2 = index_block_init(index_block, 1);
    printf("hardlinks: %i\n", index2->num_hardlinks);
    free(index2);

    // osFile* f = os_open("/folder/god/shrek.mp4", 'r');
    // printf("index block \n hardlinks: %i \n position: %u\n file size: %llu\n", f->index_block->num_hardlinks, f->index_block->position, f->index_block->file_size);
    // printf("f: \nindex blocks used: %lu\n indirect blocks used: %lu\n data blocks used: %lu\n", f->index_blocks_used, f->indirect_blocks_used, f->data_blocks_used);
    // int closed = os_close(f);    

    return 0;
}