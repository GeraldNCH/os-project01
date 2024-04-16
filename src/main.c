#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include "utils.h"

int main()
{
    char cwd[PATH_MAX];
    printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    char *new_filename = get_new_filename("test01/test/files/test-01.txt", "test/copy");
    free(new_filename);

    // copy_file("../test/files/test-01.txt", "../test/copy/test-01.txt");
    // printf("File 1 copied\n");

    // copy_file("../test/files/test-02.txt", "../test/copy/test-02.txt");
    // printf("File 2 copied\n");

    // copy_file("../test/files/test-03.txt", "../test/copy/test-03.txt");
    // printf("File 3 copied\n");

    return 0;
}