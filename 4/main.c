#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define _REENTRANT
int n;    // Размерность квадратной матрицы
//#define N_PER_PROC 100	// Кол-во строк матрицы на процесс
void init_block(int k, double *A) {
  int i, j;
  for (j = 0; j < k; j++) {
    for (i = 0; i < k; i++) {
      A[j * k + i] = (double) (rand() % 100 / (double) (100 / 2));
    }
  }
}

void init_okam(int m, double *ok_rt, double *ok_btm, double *ok_end) {
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n - m; j++) {
      ok_btm[i * (n - m) + j] = (double) (rand() % 100 / (double) (100 / 2));
      ok_rt[i * (n - m) + j] = (double) (rand() % 100 / (double) (100 / 2));
    }
  }
  for (i = 0; i < m; i++) {
    for (j = 0; j < m; j++)
      ok_end[i * m + j] = (double) (rand() % 100 / (double) (100 / 2));
  }
}

void solve_block(int k, int m, double *a, double *ok_r, double *ok_b, double *ok_e) {
  int i, j, f, s;
  double koef;
  for (i = 0; i < k; i++)
    for (j = 0; j < k; j++) {
      if ((j > i) & (a[i * k + i] != 0)) {
        koef = a[j * k + i] / a[i * k + i];
        s = i;
        for (f = i; f < k; f++)
          a[j * k + f] = a[j * k + f] - a[s * k + f] * koef;
        for (f = 0; f < m; f++)
          ok_r[j * m + f] = ok_r[j * m + f] - ok_r[s * m + f] * koef;

        //mesto dlya izmeneniya matrix B[]
      }
      //ok_btm k nulu
      if ((j < m) & (a[i * k + i] != 0)) {
        koef = ok_b[j * k + i] / a[i * k + i];
        s = i;
        for (f = i; f < k; f++)
          ok_b[j * k + f] = ok_b[j * k + f] - a[s * k + f] * koef;
        for (f = 0; f < m; f++)
          ok_e[j * m + f] = ok_e[j * m + f] - ok_r[s * m + f] * koef;
      }
    }
}

void solve_ok_end(int m, double *ok_e) {
  int i, j, f, s;
  double koef;
  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++) {
      if ((j > i) & (ok_e[i * m + i] != 0)) {
        koef = ok_e[j * m + i] / ok_e[i * m + i];
        s = i;
        for (f = i; f < m; f++)
          ok_e[j * m + f] = ok_e[j * m + f] - ok_e[s * m + f] * koef;
        //mesto dlya izmeneniya matrix B[]
      }
    }
}

void vivod_mtrx(int m, int k, int total, int myrank, double *a, double *ok_rt, double *ok_btm, double *ok_end) {
  int i, j, r = 0;
  while (r != total) {
    if (r == myrank)
      for (j = 0; j < k; j++) {
        for (i = 0; i < k * myrank; i++)
          printf("%+.2f ", 0.0);
        for (i = 0; i < k; i++) {
          printf("%+.2f ", a[j * k + i]);
        }
        for (i = 0; i < k * (total - myrank - 1); i++)
          printf("%+.2f ", 0.0);
        for (i = 0; i < m; i++)
          printf("%+.2f ", ok_rt[j * m + i]);
        printf("\n");
      }
    r++;
    MPI_Barrier(MPI_COMM_WORLD);
  }
  for (j = 0; j < m; j++) {
    r = 0;
    while (r != total) {
      if (r == myrank)
        for (i = 0; i < k; i++)
          printf("%+.2f ", ok_btm[j * k + i]);
      r++;
      MPI_Barrier(MPI_COMM_WORLD);
    }
    if (!myrank) {
      for (i = 0; i < m; i++)
        printf("%+.2f ", ok_end[j * m + i]);
      printf("\n");
    }
  }
}

int main(int argc, char **argv) {
  int myrank, total;

  double *A, *B, *C;    // Используются только в root
  double *ok_rt, *ok_btm, *ok_end;
  double *a, *b, *c;    // Лента матрицы [mxn], вектор [n], рез-т [m]
  double *ok_r, *ok_b, *ok_e;
  int k;
  int m;    // Ширина горизонтальной ленты матрицы
  int i, j;

  struct timeval start, end, ex;

  srand(time(NULL));

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &total);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  printf("Total=%d, rank=%d\n", total, myrank);
  gettimeofday(&start, 0);

  if (!myrank) {
    // Подготовка исх. данных (только root)
    k = atoi(argv[1]);   //razmer diag bloka
    m = atoi(argv[2]);   //shirina okaimleniya
    n = k * total + m;
    ok_rt = (double *) malloc(sizeof(double) * (n - m) * m);
    ok_btm = (double *) malloc(sizeof(double) * (n - m) * m);
    ok_end = (double *) malloc(sizeof(double) * m * m);
    B = (double *) malloc(sizeof(double) * n);
    C = (double *) malloc(sizeof(double) * n);
    // Инициализация матрицы A и вектора B

    for (i = 0; i < n; i++)
      B[i] = (double) i;

    init_okam(m, ok_rt, ok_btm, ok_end);

  }
  MPI_Barrier(MPI_COMM_WORLD);
  //printf("%d\n",myrank);
  MPI_Bcast((void *) &m, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast((void *) &k, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast((void *) &n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  a = (double *) malloc(sizeof(double) * k * k);
  b = (double *) malloc(sizeof(double) * n);
  c = (double *) malloc(sizeof(double) * n);

  ok_r = (double *) malloc(sizeof(double) * m * k);
  ok_b = (double *) malloc(sizeof(double) * m * k);
  ok_e = (double *) malloc(sizeof(double) * m * m);
//    ok_end = (double *) malloc (sizeof(double)*m*m);
  if (!myrank) {  // Лишнее действие, B не нужен
    memcpy(ok_e, ok_end, sizeof(double) * m * m);
  };

  init_block(k, a);

  MPI_Scatter((void *) ok_rt, m * k, MPI_DOUBLE, (void *) ok_r, m * k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter((void *) ok_btm, m * k, MPI_DOUBLE, (void *) ok_b, m * k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  vivod_mtrx(m, k, total, myrank, a, ok_r, ok_b, ok_end);
  if (!myrank)
    printf("----------------//-----------\n");

  solve_block(k, m, a, ok_r, ok_b, ok_e);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Reduce(ok_e, ok_end, m * m, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (!myrank)
    solve_ok_end(m, ok_end);

  vivod_mtrx(m, k, total, myrank, a, ok_r, ok_b, ok_end);

  MPI_Barrier(MPI_COMM_WORLD);
  gettimeofday(&end, 0);
  printf("time=%u.%u  myrank%d\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec, myrank);
  MPI_Finalize();

  exit(0);
}