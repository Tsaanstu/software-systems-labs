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
#define PATH_SIZE 64 // размер пути
#define RM "rm " // стандартная утилита удаления (для вызова в системе)
#define TAR "tar -x -f " // утилита архиватор
#define TAR_C 128 // длина команды tar

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

static void _mkdir(const char *dir) { // функция рекурсивно создающая директорию на сервере
  char tmp[256];
  char *p = NULL;
  size_t len;
  snprintf(tmp, sizeof(tmp), "%s", dir);
  len = strlen(tmp);
  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0;
  }
  for (p = tmp + 1; *p; p++)
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);
}

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

int main() {
  struct sockaddr_in sin;
  struct sockaddr_in from_sin;
  signal(SIGINT, &siginthandler); // установка обработчика SIGINT
  s = socket(AF_INET, SOCK_STREAM, 0); // установка сокета
  memset((char *) &sin, '\0', sizeof(sin)); // обнуление структуры
  sin.sin_family = AF_INET; // семейство AF_INET
  sin.sin_addr.s_addr = htons(INADDR_ANY);
  sin.sin_port = htons(SRV_PORT); // установка порта сервера
  bind(s, (struct sockaddr *) &sin, sizeof(sin));
  listen(s, 3); // очередь на сервер – 3 клиента

  while (1) {
    printf("wait...\n");
    from_len = sizeof(from_sin);
    s_new = accept(s, (struct sockaddr *) &from_sin, &from_len);
    path_l = read(s_new,
                  path, PATH_SIZE);
    write(1, path, strlen(path));
    write(1, " - writing...\n", 14);
    if (stat(path, &st) == -1) { // если директории нет - создать
      _mkdir(path);
    }

    printf("______________\npath: %s\n______________\n", path);
    strcat(path, "/tar.tar"); // создаем получаемый архив
    write(1, path, strlen(path));
//    if ((fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR) == -1)) {
//      write(1, "File error\n", 11);
//      return 1;
//    }
    fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd == -1) {
      write(1, "File error\n", 11);
      return 1;
    }
    while ((from_len = read(s_new, buf, BUF_SIZE)) != 0) {
      write(fd, buf, from_len); // записываем данные в файл tar.tar
    }
    close(fd); // закрываем файл
    fd = 0;
    memcpy(tar, TAR, strlen(TAR));
    memcpy(tar + strlen(TAR), path, strlen(path));
    memcpy(tar + strlen(TAR) + strlen(path), " -C ", 4);
    memcpy(tar_path, path, strlen(path));
    *(strrchr(tar_path, '/') + 1) = '\0';
    memcpy(tar + strlen(TAR) + strlen(path) + 4, tar_path, strlen(tar_path));
// write(1,tar,strlen(tar));
    system(tar); // создание команды распаковки и ее выполнение
    memcpy(rm_tar, RM, strlen(RM)); // удаление архива
    memcpy(rm_tar + strlen(RM), path, strlen(path));
    memset(path, '\0', PATH_SIZE);
    system(rm_tar);
    shutdown(s_new, 0); // отключение подключения
    close(s_new); // закрываем дескриптор
    write(1, "Ready!\n", 7);
    s_new = 0;
  }
}