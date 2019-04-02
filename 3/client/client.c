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
#include <arpa/inet.h>
#include <string.h>

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
#define ERROR -1

int main(int argc, char **argv) {
  /*объявляем сокет*/

  if (argc < 3) {
    perror("Bad arguments. Example: cprem path.to.src.file host@path.to.dst.dir\n");
    return ERROR;
  }

  char buf[BUF_SIZE]; // буфер
  int count;
  char tar[TAR_C]; // команда архивации
  int fd = 0;

  char *path_to_file = (char *) malloc(strlen(argv[1]));
  if (path_to_file == NULL) {
    perror("Memory error\n");
    return ERROR;
  }
  strcpy(path_to_file, argv[1]);
  printf("path to file: %s\n", path_to_file);

  size_t port = atoi(argv[2]);
  printf("port: %ld\n", port);

  char *url = (char *) malloc(strlen(strchr(argv[2], '@') + 1));
  if (path_to_file == NULL) {
    perror("Pars error\n");
    return ERROR;
  }
  strcpy(url, strchr(argv[2], '@') + 1);
  printf("url: %s\n", url);

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
  write(1, path_to_file, strlen(path_to_file));

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


  /* закрываем соединения для посылки данных */
//  if (shutdown(s, 1) < 0) {
//    perror("Error calling shutdown");
//    return 0;
//  }

//  /* читаем ответ сервера */
//  fd_set readmask;
//  fd_set allreads;
//  FD_ZERO(&allreads);
//  FD_SET(0, &allreads);
//  FD_SET(s, &allreads);


  //  for (;;) {
//    readmask = allreads;
//    if (select(s + 1, &readmask, NULL, NULL, NULL) <= 0) {
//      perror("Error calling select");
//      return 0;
//    }
//    if (FD_ISSET(s, &readmask)) {
//      char buffer[20];
//      memset(buffer, 0, 20 * sizeof(char));
//      int result = recv(s, buffer, sizeof(buffer) - 1, 0);
//      if (result < 0) {
//        perror("Error calling recv");
//        return 0;
//      }
//      if (result == 0) {
//        perror("Server disconnected");
//        return 0;
//      }
//      if (strncmp(buffer, "Hi, dear!", 9) == 0)
//        printf("Got answer. Success.\n");
//      else
//        perror("Wrong answer!");
//    }
//    if (FD_ISSET(0, &readmask)) {
//      printf("No server response");
//      return 0;
//    }
//  }

  return 0;
}






//int main(int argc, char **argv) {
//  int s;
//  int from_len;
//  char path[PATH_SIZE]; // путь
//  char *path_p;
//  int fd;
//  char tar[TAR_C]; // команда архивации
//  char host[TAR_C]; // адрес сервера
//  int size;
//  struct hostent *hp = NULL;
//  struct sockaddr_in clnt_sin, srv_sin;
//
//  s = socket(AF_INET, SOCK_STREAM, 0);
//  memset((char *) &clnt_sin, '\0', sizeof(clnt_sin));
//
//  clnt_sin.sin_family = htons(AF_INET);
//  clnt_sin.sin_addr.s_addr = INADDR_ANY;
//  clnt_sin.sin_port = htons(CLNT_PORT);
//
//  bind(s, (struct sockaddr *) &clnt_sin, sizeof(clnt_sin));
//  memcpy(host, argv[2], strlen(argv[2]));
//
//  * strchr(host, '@') = '\0';
//  memset((char *) &srv_sin, '\0', sizeof(srv_sin));
//  if ((hp = gethostbyname(host)) == NULL) { // получаем host
//    write(1, "Can't find ", 11);
//    write(1, host, strlen(host));
//    write(1, "\n", 1);
//    return 1;
//  }
//  srv_sin.sin_family = htons(AF_INET);
////  memcpy((char *) &srv_sin.sin_addr, hp->h_addr, hp->h_length);
//  srv_sin.sin_port = htons(SRV_PORT);
//  srv_sin.sin_addr.s_addr = inet_addr( "127.0.0.1" );
//
//  if (connect(s, (struct sockaddr *) &srv_sin, sizeof(srv_sin))) { // подключение
//    write(1, "Can't connect to ", 17);
//    write(1, host, strlen(host));
//    write(1, "\n", 1);
//    return 1;
//  }
//
//// парсинг аргументов программы
//  path_p=strstr(argv[2], "@");
//  path_p++;
//  memcpy(path, path_p, strlen(path_p));
//  write(1, path, strlen(path));

//  return (0);
//}