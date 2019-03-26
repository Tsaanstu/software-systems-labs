//
// Created by tsaanstu on 25.03.19.
//

#include "definitions.h"
#include "st.h"

void *second_thread(void *arg_p) {
  write(1, "start: second thread\n", 21);
  while (1) {
    switch (modes[1]) {
      case 1:st_translator();
        break;
      case 2:string_toupper();
        break;
      case 3:string_tolower();
        break;
      case 4:string_change_register();
        break;
      default:break;
    }
  }
}

void st_translator() {
  char *str = "second thread mode: 1\n";
  int i, j, l;
  pthread_mutex_lock(&mutx2);
  if (modes[1] != 1) {
    pthread_mutex_unlock(&mutx2);
    return;
  }
  write(1, str, strlen(str));
  write(1, str_in_second_thread, second_len);
  memset(str_in_second_thread, '\0', BUF_SIZE);
  second_len = 0;
  pthread_mutex_unlock(&mutx2);
  pthread_cond_signal(&condx2);
  usleep(10);
}

void string_toupper() {
  char c, buf[BUF_SIZE], *str = "second thread mode: 2\n";;
  int i, j, len;
  pthread_mutex_lock(&mutx2);
  if (modes[1] != 2) {
    pthread_mutex_unlock(&mutx2);
    return;
  }
  write(1, str, strlen(str));

  strncpy(buf, str_in_second_thread, second_len);
  memset(str_in_second_thread, '\0', BUF_SIZE);
  len = second_len;
  second_len = 0;
  pthread_mutex_unlock(&mutx2);
  pthread_cond_signal(&condx2);
  usleep(10);

  for (i = 0; i < len; i++) {
    buf[i] = toupper(buf[i]);
  }
  write(1, buf, len);
}

void string_tolower() {
  char c, buf[BUF_SIZE], *str = "second thread mode: 3\n";;
  int i, j, len;
  pthread_mutex_lock(&mutx2);
  if (modes[1] != 3) {
    pthread_mutex_unlock(&mutx2);
    return;
  }
  write(1, str, strlen(str));

  strncpy(buf, str_in_second_thread, second_len);
  memset(str_in_second_thread, '\0', BUF_SIZE);
  len = second_len;
  second_len = 0;
  pthread_mutex_unlock(&mutx2);
  pthread_cond_signal(&condx2);
  usleep(10);

  for (i = 0; i < len; i++) {
    buf[i] = tolower(buf[i]);
  }
  write(1, buf, len);
}

void string_change_register() {
  char c, buf[BUF_SIZE], *str = "second thread mode: 4\n";;
  int i, j, len;
  pthread_mutex_lock(&mutx2);
  if (modes[1] != 4) {
    pthread_mutex_unlock(&mutx2);
    return;
  }
  write(1, str, strlen(str));

  strncpy(buf, str_in_second_thread, second_len);
  memset(str_in_second_thread, '\0', BUF_SIZE);
  len = second_len;
  second_len = 0;
  pthread_mutex_unlock(&mutx2);
  pthread_cond_signal(&condx2);
  usleep(10);

  for (i = 0; i < len; i++) {
    if (islower(buf[i])) {
      buf[i] = toupper(buf[i]);
    } else if (isupper(buf[i])) {
      buf[i] = tolower(buf[i]);
    }
  }
  write(1, buf, len);
}