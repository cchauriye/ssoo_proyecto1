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


extern char* diskname;

int main(int argc, char *argv[])
{
    create_test_bin();
    os_mount(argv[1]);
    os_exists(argv[1]);
    os_bitmap(1, false);
    // os_ls("");
    // os_bitmap(1, false);
    // print_all_entries_from_dir(536679); //536679
    os_mkdir("folder/a10/b3");
    os_bitmap(1, false);
    // Dir_block* root_dir = dir_block_init(0);
    // Dir_block_entry* first_entry = dir_block_entry_init(root_dir, 0);
    // unsigned int block_num = find_dir_entry_by_name(root_dir, "folder");
    // print_all_entries_from_dir(70);
    // free(root_dir);
    // free(first_entry);

    // long int buff_size = 200;
    // unsigned char buffer[buff_size];
    // read_from_position(0, buffer, buff_size);
    // print_binary_buffer(buffer, buff_size);
    return 0;
}