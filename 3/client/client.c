#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#define SRV_PORT 1234  // порт сервера
#define CLNT_PORT 1235  // порт клиента
#define BUF_SIZE 4096  // размер буфера
#define PATH_SIZE 64  // размер пути

// размер команды архиватора
#define RM "rm ./tar.tar"
// команда удаления архива
#define TAR "tar -c -f ./tar.tar"
// команда архивации
#define ERROR -1

int main(int argc, char **argv) {
  /*объявляем сокет*/

  if (argc < 3) {
    perror("Bad arguments. Example: cprem path.to.src.file host@path.to.dst.dir\n");
    return ERROR;
  }

  char *buf = calloc(BUF_SIZE, sizeof(char *)); // буфер
  int count;

//  printf("cmd: %s\n", TAR);
  size_t TAR_C = strlen(TAR) + strlen(argv[1]);

  char tar[TAR_C]; // команда архивации
  int fd = 0;

  char *path_to_file = (char *) malloc(strlen(argv[1]));
  if (path_to_file == NULL) {
    perror("Memory error\n");
    return ERROR;
  }
  strcpy(path_to_file, argv[1]);
//  printf("path to file: %s\n", path_to_file);

  size_t port = atoi(argv[2]);
//  printf("port: %ld\n", port);

  char *url = (char *) malloc(strlen(strchr(argv[2], '@') + 1));
  if (path_to_file == NULL) {
    perror("Pars error\n");
    return ERROR;
  }
  strcpy(url, strchr(argv[2], '@') + 1);
//  printf("url: %s\n", url);

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("Error calling socket");
    return 0;
  }
  /*соединяемся по определённому порту с хостом*/
  struct sockaddr_in peer;
  peer.sin_family = AF_INET;
  peer.sin_port = htons(port);
  peer.sin_addr.s_addr = inet_addr(url);
  int result = connect(s, (struct sockaddr *) &peer, sizeof(peer));
  if (result) {
    perror("Error calling connect");
    return 0;
  }



  memcpy(tar, TAR, strlen(TAR));
  memcpy(tar + strlen(tar), argv[1], strlen(argv[1]));
  system(tar); // команда на архивацию
  write(s, path_to_file, strlen(path_to_file));

  if ((fd = open("./tar.tar", O_RDONLY)) == -1) {
    write(1, "File error\n", 11);
    return 1;
  }
  while (count = read(fd, buf, BUF_SIZE)) {
    write(s, buf, count); // передаем архив на сервер
  }
  write(1, argv[1], strlen(argv[1]));
  write(1, " is copied to the server\n", 25);
  close(fd);
   system(RM);
  close(s);
  return 0;
}