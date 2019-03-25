#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#define _REENTRANT

#include <pthread.h>
#include <sched.h>
#define BUF_SIZE 256
extern int errno;

pthread_t itid, ctid;
pthread_mutex_t mutx1, mutx2;
pthread_cond_t condx1, condx2;
int msgline1 = 0, msgline2 = 0;
int flag11 = 1, flag12 = 0, flag13 = 0, flag14 = 0, flag21 = 1, flag22 = 0, flag23 = 0, flag24 = 0;
char msgbuf1[BUF_SIZE], msgbuf2[BUF_SIZE];

void clean(char *line) {
    int j;
    for (j = 0; j < BUF_SIZE; j++)
        line[j] = '\0';
}

void func(int s) {
    char c[3];
    write(1, "\nSigcautched\n", 14);
    read(0, c, 3);
    if (!strncmp(c, "1.1", 3)) {
        flag12 = flag13 = flag14 = 0;
        flag11 = 1;
    } else if (!strncmp(c, "1.2", 3)) {
        flag11 = flag13 = flag14 = 0;
        flag12 = 1;
    } else if (!strncmp(c, "1.3", 3)) {
        flag11 = flag12 = flag14 = 0;
        flag13 = 1;
    } else if (!strncmp(c, "1.4", 3)) {
        flag11 = flag12 = flag13 = 0;
        flag14 = 1;
    } else if (!strncmp(c, "2.1", 3)) {
        flag22 = flag23 = flag24 = 0;
        flag21 = 1;
    } else if (!strncmp(c, "2.2", 3)) {
        flag21 = flag23 = flag24 = 0;
        flag22 = 1;
    } else if (!strncmp(c, "2.3", 3)) {
        flag22 = fl
        ag21 = flag24 = 0;
        flag23 = 1;
    } else if (!strncmp(c, "2.4", 3)) {
        flag22 = flag23 = flag21 = 0;
        flag24 = 1;
    } else {
        pthread_cond_signal(&condx1);
    }
}

int trans(int sig) {
    char c, buf[BUF_SIZE];
    int i, j, l;
    pthread_mutex_lock(&mutx1);
    if (!flag11) {
        pthread_mutex_unlock(&mutx1);
        return 1;
    }
    write(1, "I1\n", 3);
    strncpy(msgbuf2, msgbuf1, msgline1);
    clean(msgbuf1);
    write(1, msgbuf2, msgline1);
    msgline2 = msgline1;
    msgline1 = 0;
    pthread_mutex_unlock(&mutx1);
    pthread_cond_sig
            nal(&condx1);
    pthread_cond_wait(&condx2, &mutx2);
    usleep(10);
}

int invert(int sig) {
    char c;
    int i, l, j;
    pthread_mutex_lock(&mutx1);
    if (!flag12) {
        pthread_mutex_unlock(&mutx1);
        return 1;
    }
    write(1, "I2\n", 3);
    strncpy(msgbuf2, msgbuf1, ms
    gline1);
    clean(msgbuf1);
    msgline2 = msgline1;
    msgline1 = 0;
    pthread_mutex_unlock(&mutx1);
    pthread_cond_signal(&condx1);
    l = msgline2;
    for (i = 0; i < (l / 2); i++) {
        c = msgbuf2[i];
        msgbuf2[i] = msgbuf2[l-2-i];
        msgbuf2[l-2-i] = c;
    }
    write(1, msgbuf2, msgline2);
    pthread_cond_wait(&condx2, &mutx2);
    usleep(10);
}

int change(int sig) {
    char c;
    int i, j, l;
    pthread_mutex_lock(&mutx1);
    if (!flag13) {
        pthread_mutex_unlock(&mutx1);
        return 1;
    }
    write(1, "I3\n", 3);
    strncpy(m
    sgbuf2, msgbuf1, msgline1);
    clean(msgbuf1);
    msgline2 = msgline1;
    msgline1 = 0;
    pthread_mutex_unlock(&mutx1);
    pthread_cond_signal(&condx1);
    l = msgline2;
    for (i = 0; i < l-2; i += 2) {
        c = msgbuf2[i];
        msgbuf2[i] = msgbuf2[i + 1];
        msgbuf2[i + 1] = c;
    };
    write(1, msgbuf2, msgline2);
    pthread_cond_wait(&condx2, &mutx2);
    usleep(10);
}

int koi(int sig) {
    char c;
    int i, l, j;
    pthread_mutex_lock(&mutx1);
    if (!flag14) {
        pthread_mutex_unlock(&mutx1);
        return 1;
    }
    write(1, "I4\n", 3);
    strncpy(msgbuf2, msgbuf1, msgline1);
    clean(msgbuf1);
    msgline2 = msgline1;
    msgline1 = 0;
    pthread_mutex_unlock(&mutx1);
    pthread_cond_signal(&condx1);
    l = msgline2;
    for (i = 0; i < l - 1; i++) {
        if (isupper(msgbuf2[i])) msgbuf2[i] += 32;
        else if (islower(msgbuf2[i]))
            msgbuf2[i] -= 32;
        msgbuf2[i] |= 1 << 7;
    };
    write(1, msgbuf2, msgline2);
    pthread_cond_wait(&condx2, &mutx2);
    usleep(10);
}

void *invertor(void *arg_p) {
    char c;
    write(1, "I start\n", 8);
    pthread_mutex_lock(&mutx2);
    while (1) {
        if (flag11) {
            trans(1);
        }
        if (flag12) {
            invert(1);
        }
        if (flag13) {
            change(1);
        }
        if (flag14) {
            koi(1);
        }
    }
}

int trans1(int sig) {
    int i, j, l;
    pthread_mutex_lock(&mutx2);
    if (!flag21) {
        pthread_mutex_unlock(&mutx
        2);
        return 1;
    }
    write(1, "C1\n", 3);
    write(1, msgbuf2[i], msgline2);
    clean(msgbuf2);
    msgline2 = 0;
    pthread_mutex_unlock(&mutx2);
    pthread_cond_signal(&condx2);
    usleep(10);
}

int up(int sig) {
    char c, buf[BUF_SIZE];
    int i, j, len;
    pthread_m
            utex_lock(&mutx2);
    if (!flag22) {
        pthread_mutex_unlock(&mutx2);
        return 1;
    }
    write(1, "C2\n", 3);
    strncpy(buf, msgbuf2, msgline2);
    clean(msgbuf2);
    len = msgline2;
    msgline2 = 0;
    pthread_mutex_unlock(&mutx2);
    pthread_cond_signal(&condx2);
    usleep(10);
    for (i = 0; i < len; i++) {
        buf[i] = toupper(buf[i]);
    }
    write(1, buf, len);
    clean(buf);
}

int down(int sig) {
    char c, buf[BUF_SIZE];
    int i, j, len, l;
    pthread_mutex_lock(&mutx2);
    if (!flag23) {
        pthread_mutex_unlock(&mutx2);
        return 1;
    }
    write(1, "C3\n", 3);
    strncpy(buf, msgbuf2, msgline2);
    clean(msgbuf2[i]);
    len = msgline2;
    msgline2 = 0;
    pthread_mutex_unlock(&mutx2);
    pthread_cond_signal(&condx2);
    usleep(10);
    for (i = 0; i < len; i++) {
        buf[i] = tolower(buf[i]);
    }
    write(1, buf, len);
    clean(buf);
}

int convert(int sig) {
    char c, buf[BUF_SIZE];
    int i, l, j, len;
    pthread_mutex_lock(&mutx2);
    if (!flag24) {
        pthread_mutex_unlock(&mutx2);
        return 1;
    }
    write(1, "C4\n", 3);
    strncpy(buf, msgbuf2, msgline2);
    clean(msgbuf2);
    len = msgline2;
    msgline2 = 0;
    pthread_mutex_unlock(&mutx2);
    pthread_cond_signal(&condx2);
    usleep(10);
    for (i = 0; i < len; i++) {
        if (isupper(buf[i]))
            buf[i] = tolower(buf[i]);
        else
            buf[i] = toupper(buf[i]);
    };
    write(1, buf, len);
    clean(buf);
}

void *consumer(void *arg_p) {
    write(1, "C start\n", 8);
    while (1) {
        if (flag21) {
            trans1(1);
        }
        if (flag22) {
            up(1);
        }
        if (flag23) {
            down(1);
        }
        if (flag24) {
            convert(1);
        }
    };
}

int main() {
    pthread_attr_t pattr;
    int ret;
    write(1, "P start\n", 8);
    pthread_attr_init(&pattr);
    pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);
    pthread_mutex_init(&mutx1, NULL);
    pthread_cond_init(&condx1, NULL);
    pthread_mutex_init(&mutx2, NULL);
    pthread_cond_init(&condx2, NULL);
    if (ret = pthread_create(&itid, &pattr, invertor, NULL))
        perror("pthread_create");
    if (ret = pthread_create(&ctid, &pattr, consumer, NULL))
        perror("pthread_create");
    signal(SIGINT, func);
    pthread_mutex_lock(&mutx1);
    while (1) {
        msgline1 = read(0, msgbuf1, BUF_SIZE);
        if (msgbuf1[0] == 27) raise(SIGQUIT);
        if (msgbuf1[0] != '\n')
        pthread_cond_wait(&condx1, &mutx1);
    }
}