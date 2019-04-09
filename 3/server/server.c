#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <memory.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define SRV_PORT 1234 // порт подключения
#define BUF_SIZE 4096 // размер буфера
#define PATH_SIZE 128 // размер пути
#define RM "rm file_storage.tar" // стандартная утилита удаления (для вызова в системе)
#define TAR "tar -xvf file_storage.tar" // утилита архиватор
#define TAR_C 256 // длина команды tar

struct stat st = {0};
int s, s_new = 0;
int from_len;
int fd = 0;
char buf[BUF_SIZE];
char path[PATH_SIZE];
char tar[TAR_C];
char rm_tar[TAR_C];
char tar_path[TAR_C];
int path_l, name_l;

struct sockaddr_in c_addr;

void gotoxy(int x, int y) {
  printf("%c[%d;%df", 0x1B, y, x);
}

//void *get_file(int *arg) {
//  char recvBuff[1024];
//  int sockfd = (int) *arg;
//  FILE *fp;
//  char fname[100];
//  int bytesReceived = 0;
//  read(sockfd, fname, 256);
//  printf("File Name: %s :)\n", fname);
//  printf("Receiving file...");
//
//  fp = fopen(fname, "ab");
//
//  if (NULL == fp) {
//    printf("Error opening file\n");
//    return 1;
//  }
//
//  long double sz = 1;
//  while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
//    sz++;
//    gotoxy(0, 4);
//    printf("Received: %llf Mb", (sz / 1024));
//    fflush(stdout);
//    fwrite(recvBuff, 1, bytesReceived, fp);
//  }
//
//  if (bytesReceived < 0) {
//    printf("\n Read Error \n");
//  }
//  printf("\nFile OK....Completed\n");
//  fclose(fp);
//}

void siginthandler() { // обработчик Ctrl+C, грамотное отключение сервера
  write(1, "\nServer is shutting down\n", 25);
  if (fd) {
    close(fd);
  }
  shutdown(s_new, 0);
  if (s_new) {
    close(s_new);
  }
  exit(0);
}

int main(int argc, char *argv[]) {
  system("clear");
  int connfd = 0, err;
  pthread_t tid;
  struct sockaddr_in serv_addr;
  int listenfd = 0, ret;
  char sendBuff[1025];
  int numrv;
  size_t clen = 0;
  signal(SIGINT, &siginthandler);
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    printf("Error in socket creation\n");
    exit(2);
  }

  printf("Socket retrieve success\n");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(SRV_PORT);

  ret = bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    printf("Error in bind\n");
    exit(2);
  }

  if (listen(listenfd, 10) == -1) {
    printf("Failed to listen\n");
    return -1;
  }

  while (1) {
    clen = sizeof(c_addr);
    printf("Waiting...\n");
    connfd = accept(listenfd, (struct sockaddr *) &c_addr, &clen);
    if (connfd < 0) {
      printf("Error in accept\n");
      continue;
    }
    char *recvBuff = (char *) malloc(1024);
    int sockfd = connfd;
    FILE *fp;
    char *fname = (char *) malloc(100);
    strcpy(fname, "file_storage.tar");
    int bytesReceived = 0;
//    read(sockfd, fname, 256);
    printf("File Name: %s :)\n", fname);
    printf("Receiving file...");

    fp = fopen(fname, "ab");

    if (NULL == fp) {
      printf("Error opening file\n");
      return 1;
    }

    long double sz = 1;
    while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
      sz++;
      gotoxy(0, 4);
      printf("Received: %llf Mb", (sz / 1024));
      fflush(stdout);
      fwrite(recvBuff, 1, bytesReceived, fp);
    }

    if (bytesReceived < 0) {
      printf("\n Read Error \n");
    }

    printf("1) %s\n2) %s\n", tar, rm_tar);
    system(TAR); // создание команды распаковки и ее выполнение
    system(RM);

    printf("\nFile OK....Completed\n");
    fclose(fp);
  }
//  while (1) {
//    clen = sizeof(c_addr);
//    printf("Waiting...\n");
//    connfd = accept(listenfd, (struct sockaddr *) &c_addr, &clen);
//    if (connfd < 0) {
//      printf("Error in accept\n");
//      continue;
//    }
//    err = pthread_create(&tid, NULL, &get_file, &connfd);
//    if (err != 0)
//      printf("\ncan't create thread :[%s]", strerror(err));
//  }
  close(connfd);
  return 0;
}


//    system(tar); // создание команды распаковки и ее выполнение
//    memcpy(rm_tar, RM, strlen(RM)); // удаление архива
//    memcpy(rm_tar + strlen(RM), path, strlen(path));
//    memset(path, '\0', PATH_SIZE);
//    system(rm_tar);