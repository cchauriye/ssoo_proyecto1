#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h> 
#include <stdlib.h>
#include "API/os_API.h"

extern char* diskname;

int main(int argc, char *argv[])
{
    // create_test_bin();
    // os_mount(argv[1]);
    // printf("%s\n", diskname);
    // os_exists(argv[1]);
    // os_bitmap(1, false);
    dir_name_from_path("hola/como/estas.x");
    return 0;
}