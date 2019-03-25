//
// Created by tsaanstu on 25.03.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int logical_expression(int x, int y, int z) {
    return x && y && z;
}

int main(int argc, char **argv) {
    int pid1 = 0, pid2 = 0, pid3 = 0, answer = 0;
    pid1 = fork();
    pid2 = fork();
    pid3 = fork();
    answer = logical_expression(pid1, pid2, pid3);
    printf("%d %d %d = %d\n", pid1, pid2, pid3, answer);
    while (wait(NULL) > 0);
    return 0;
}