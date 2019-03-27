//
// Created by tsaanstu on 25.03.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 4

int logical_expression(int *pid) {
  return pid[0] && pid[1] && pid[2] && pid[3];
}

int main(int argc, char **argv) {
  int *pid = (int*)calloc(SIZE, sizeof(int));
  for (int i = 0; i < SIZE; ++i) {
    pid[i] = fork();
  }
  int answer = logical_expression(pid);
  for (int i = 0; i < SIZE; ++i) {
    printf("%d ", pid[i]);
  }
  printf("= %d\n", answer);
  while (wait(NULL) > 0);
  return 0;
}