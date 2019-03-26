//
// Created by tsaanstu on 25.03.19.
//

#include "definitions.h"
#include "ft.h"
#include "st.h"

unsigned short int modes[2];
pthread_t itid, ctid;
pthread_mutex_t mutx1, mutx2;
pthread_cond_t condx1, condx2;
char str_in_first_thread[BUF_SIZE];
unsigned int first_len;
unsigned int second_len;

void read_commands();

int main(int argc, char **argv) {
  pthread_attr_t pattr;
  int ret;
  write(1, "start: main thread\n", 19);
  pthread_attr_init(&pattr);
  pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
  pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutx1, NULL);
  pthread_cond_init(&condx1, NULL);
  pthread_mutex_init(&mutx2, NULL);
  pthread_cond_init(&condx2, NULL);
  if (ret = pthread_create(&itid, &pattr, first_thread, NULL)) {
    perror("pthread_create");
  }
  if (ret = pthread_create(&ctid, &pattr, second_thread, NULL)) {
    perror("pthread_create");
  }

  signal(SIGINT, read_commands);
  pthread_mutex_lock(&mutx1);
  while (1) {
    first_len = read(0, str_in_first_thread, BUF_SIZE);
    if (str_in_first_thread[0] == '~') {
      exit(0);
    }
    if (str_in_first_thread[0] != '\n') {
      pthread_cond_wait(&condx1, &mutx1);
    }
  }
}

void read_commands() {
  char buf[3];
  char *answer[] = {"\n\n\nsignal \"ctrl+c\" is processed\n",
                    "input first and second thread mode (two 1-4 int numbers)\nExample: 14\n",
                    "input error\n"
  };
  write(1, answer[0], strlen(answer[0]));
  unsigned short int mode = 0;
  size_t i = 0;
  write(1, answer[1], strlen(answer[1]));
  read(0, buf, 3);

  for (i = 0; i < 2; i++) {
    mode = buf[i] - '0';
    if (mode < 1 || mode > 4) {
      write(1, answer[2], strlen(answer[2]));
      exit(1);
    }
    modes[i] = mode;
  }
}