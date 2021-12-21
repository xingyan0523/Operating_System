#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
    memset(argv[0], 0, strlen(argv[0]));
    strcpy(argv[0], "ls");
    execvp("ls", argv);
    return 0;
}