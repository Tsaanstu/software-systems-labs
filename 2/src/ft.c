//
// Created by tsaanstu on 25.03.19.
//

#include "ft.h"

void *first_thread(void *arg_p) {
  write(0, "start: first thread\n", 20);
  while (1) {
    switch (modes[0]) {
      case 1:ft_translator();
        break;
      default:break;
    }
  }
}

void ft_translator() {
  char c, buf[BUF_SIZE], *str = "first thread mode: 1\n";
  int i, j, l;
  pthread_mutex_lock(&mutx1);
  if (modes[0] != 1) {
    pthread_mutex_unlock(&mutx1);
    return;
  }
  write(1, str, strlen(str));
  second_len = first_len;
  first_len = 0;
  strncpy(str_in_second_thread, str_in_first_thread, second_len);

  memset(str_in_first_thread, '\0', BUF_SIZE);

  write(1, str_in_second_thread, second_len);

  pthread_mutex_unlock(&mutx1);
  pthread_cond_signal(&condx1);
  pthread_cond_wait(&condx2, &mutx2);
  usleep(10);
}