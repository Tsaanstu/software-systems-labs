//
// Created by tsaanstu on 27.03.19.
//


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

#define SRV_PORT 1234  // порт сервера
#define CLNT_PORT 1235  // порт клиента
#define BUF_SIZE 4096  // размер буфера
#define PATH_SIZE 64  // размер пути

#define TAR_C 64
// размер команды архиватора
#define RM "rm ./tar.tar"
// команда удаления архива
#define TAR "tar -c -f ./tar.tar "
// команда архивации

int main(int argc, char **argv) {
  int s;
  int from_len;
  char buf[BUF_SIZE]; // буфер
  char path[PATH_SIZE]; // путь
  char *path_p;
  int fd;
  char tar[TAR_C]; // команда архивации
  char host[TAR_C]; // адрес сервера
  int count;
  int size;
  struct hostent *hp = NULL;
  struct sockaddr_in clnt_sin, srv_sin;

  s = socket(AF_INET, SOCK_STREAM, 0);
  memset((char *) &clnt_sin, '\0', sizeof(clnt_sin));
  clnt_sin.sin_family = AF_INET;
  clnt_sin.sin_addr.s_addr = INADDR_ANY;
  clnt_sin.sin_port = CLNT_PORT;
  bind(s, (struct sockaddr *) &clnt_sin, sizeof(clnt_sin));
  memcpy(host, argv[2], strlen(argv[2]));

  * strchr(host, '@') = '\0';
  memset((char *) &srv_sin, '\0', sizeof(srv_sin));
  if ((hp = gethostbyname(host)) == NULL) { // получаем host
    write(1, "Can't find ", 11);
    write(1, host, strlen(host));
    write(1, "\n", 1);
    return 1;
  }
  srv_sin.sin_family = AF_INET;
  memcpy((char *) &srv_sin.sin_addr, hp->h_addr, hp->h_length);
  srv_sin.sin_port = SRV_PORT;
  if (connect(s, (struct sockaddr *) &srv_sin, sizeof(srv_sin)) == -1) { // подключение
    write(1, "Can't connect to ", 17);
    write(1, host, strlen(host));
    write(1, "\n", 1);
    return 1;
  }

// парсинг аргументов программы
  path_p=strstr(argv[2], "@");
  path_p++;
  memcpy(path, path_p, strlen(path_p));
  write(1, path, strlen(path));
  memcpy(tar, TAR, strlen(TAR));
  memcpy(tar + strlen(tar), argv[1], strlen(argv[1]));
  system(tar); // команда на архивацию
  write(s, path, strlen(path));
  write(1, path, strlen(path));

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
// удаляем архив на клиенте
  close (s);
  return (0);
}