//
// Created by tsaanstu on 25.03.19.
//

#ifndef SOFTWARE_SYSTEMS_LABS_DEFINITIONS_H
#define SOFTWARE_SYSTEMS_LABS_DEFINITIONS_H

#define BUF_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

inline unsigned short int modes[2];
inline pthread_t itid, ctid;
inline pthread_mutex_t mutx1, mutx2;
inline pthread_cond_t condx1, condx2;
inline char str_in_first_thread[BUF_SIZE];
inline char str_in_second_thread[BUF_SIZE];
inline unsigned int first_len;
inline unsigned int second_len;

#endif //SOFTWARE_SYSTEMS_LABS_DEFINITIONS_H
