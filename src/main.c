#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include "copy-file.h"

int main()
{
    char cwd[PATH_MAX];
    printf("CWD: %s\n", getcwd(cwd, sizeof(cwd)));

    // copy_file("../test/test-01.txt", "../copy-test/");
    // printf("File 1 copied\n");

    copy_file("../test/test-02.txt", "../copy-test/");
    printf("File 2 copied\n");

    // copy_file("../test/test-03.txt", "../copy-test/");
    // printf("File 3 copied\n");

    return 0;
}