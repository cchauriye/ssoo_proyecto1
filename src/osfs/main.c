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
    // Probar mkdir
    // os_bitmap(1, false);
    // os_mkdir("/folder/god/chaurri1");
    // os_ls("/folder/god");
    // os_bitmap(1, false);

    return 0;
}