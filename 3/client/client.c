#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define SRV_PORT 1234
#define CLNT_PORT 1235
#define RM "rm file_storage.tar"
#define TAR "tar -c -f ./file_storage.tar "

int main(int argc, char **argv) {
  system("clear");
  int sockfd = 0;
  int bytesReceived = 0;
  char recvBuff[1024];
  memset(recvBuff, '0', sizeof(recvBuff));
  struct sockaddr_in serv_addr;

  if (argc < 3) {
    perror("Bad arguments. Example: cprem path.to.src.file host@path.to.dst.dir\n");
    return 1;
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("\n Error : Could not create socket \n");
    return 1;
  }

  char ip[16];
  strcpy(ip, argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SRV_PORT);
  serv_addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    printf("\n Error : Connect Failed \n");
    return 1;
  }

  printf("Connected to ip: %s : %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

  size_t TAR_C = strlen(TAR) + strlen(argv[2]);
  char *tar = (char *) malloc(TAR_C);
  memcpy(tar, TAR, strlen(TAR));
  memcpy(tar + strlen(tar), argv[2], strlen(argv[2]));
  system(tar);

  FILE *fp = fopen("file_storage.tar", "rb");
  if (fp == NULL) {
    printf("File open error");
    return 1;
  }

  printf("Sending...\n");
  while (1) {
    unsigned char buff[1024] = {0};
    int nread = fread(buff, 1, 1024, fp);

    if (nread > 0) {
      write(sockfd, buff, nread);
    }

    if (nread < 1024) {
      if (feof(fp)) {
        printf("End of file\nSending done\n");
      }
      if (ferror(fp))
        printf("Error reading\n");
      break;
    }
  }

  printf("Closing Connection\n");
  close(sockfd);
  shutdown(sockfd, SHUT_WR);
  system(RM);
  return 0;
}