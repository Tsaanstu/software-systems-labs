#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define SRV_PORT 1234
#define RM "rm file_storage.tar"
#define TAR "tar -xvf file_storage.tar"

struct stat st = {0};
int s, s_new = 0;
int fd = 0;
struct sockaddr_in c_addr;
int connfd;

void upload_text(int x, int y);
void siginthandler();

int main(int argc, char *argv[]) {
  system("clear");
  connfd = 0;
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
    printf("Receiving file...");

    fp = fopen(fname, "ab");

    if (NULL == fp) {
      printf("Error opening file\n");
      return 1;
    }

    long double sz = 1;
    while ((bytesReceived = read(sockfd, recvBuff, 1024)) > 0) {
      sz++;
      upload_text(0, 4);
      printf("Received: %llf Mb", (sz / 1024));
      fflush(stdout);
      fwrite(recvBuff, 1, bytesReceived, fp);
    }

    if (bytesReceived < 0) {
      printf("\n Read Error \n");
    }

    system(TAR);
    system(RM);

    printf("\nFile OK...\nCompleted\n");
    fclose(fp);
  }
  return 0;
}

void upload_text(int x, int y) {
  printf("%c[%d;%df", 0x1B, y, x);
}

void siginthandler() {
  write(1, "\nServer is shutting down\n", 25);
  close(connfd);
  if (fd) {
    close(fd);
  }
  shutdown(s_new, 0);
  if (s_new) {
    close(s_new);
  }
  exit(0);
}







//  thread?
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
//    upload_text(0, 4);
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