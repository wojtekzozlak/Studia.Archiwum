#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "f2c.h"
#include "clapack.h"

#define __DOUBLE__
#ifndef __DOUBLE__
  #define FLOAT float
  #define GESV sgesv_
#else
  #define FLOAT double
  #define GESV dgesv_
  #define GELQF dgelqf_
  #define ORGLQ dorglq_
  #define ORMLQ dormlq_
  #define GELS dgels_
  #define GELSY dgelsy_
#endif

#define IND(n,i,j) ((n)*(j)+(i))
#define CHECK_INFO(x, s) if(x != 0){ printf("\n\nCritical error in: \n\n"); printf(s); printf("\nvalue: %ld\n", x); exit(-1); };


void wczytaj_macierz(long int m, long int n, FLOAT *x)
{
  long int i, j;
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++)
      scanf("%lf", &x[IND(m,i,j)]);
  return ;
}

void wypisz_macierz(long int m, long int n, FLOAT *x)
{
  long int i, j;
  for(i = 0; i < m; i++){
    for(j = 0; j < n; j++)
      printf("%f ", x[IND(m,i,j)]);
    printf("\n");
  }
  return ;
}


void wczytywanie(long int *m, long int *k, long int *n, FLOAT **A, FLOAT **d, FLOAT **e)
{
  printf("Podaj wymiary macierzy B (m x n) oraz C (k x n)\n");
  printf("m: ");
  scanf("%ld", m);
  printf("k: ");
  scanf("%ld", k);
  printf("n: ");
  scanf("%ld", n);

  *A = malloc(sizeof(FLOAT) * (*m + *k) * (*n));
  *d = malloc(sizeof(FLOAT) * (*m));
  *e = malloc(sizeof(FLOAT) * (*k));

  printf("Podaj współczynniki macierzy A:\n");
  wczytaj_macierz(*m + *k, *n, *A);
  printf("Wektora d:\n");
  wczytaj_macierz(*m, 1, *d);
  printf("Wektora e:\n");
  wczytaj_macierz(*k, 1, *e);

  return ;
}

void policz(
      long int m, long int k, long int n,
      FLOAT *A, FLOAT *d, FLOAT *e, FLOAT *y)
{
  FLOAT *B, *LQ, *_LA_tau, *_LA_work, buf = 0, *L, sum, _LA_rcond = 1.0e-10;
  long int info, _LA_lwork, *_LA_permut, _LA_nrhs = 1, i, j, dim, *_LA_jpvt, _LA_rank;
  char _LA_side, _LA_trans;

  /* kopia danych - bez magicznego operowania na jednej macierzy, *
   * żeby się nie pobugić                                         */
  dim = m + k;
  B = malloc(sizeof(FLOAT) * dim * n);
  LQ = B + m * n; /* LQ = C, a później zawiera rozkład LQ macierzy C*/
  for(i = 0; i < m; i++)
    for(j = 0; j < n; j++)
      B[IND(m,i,j)] = A[IND(dim,i,j)];

  for(i = 0; i < k; i++)
    for(j = 0; j < n; j++)
      LQ[IND(k,i,j)] = A[IND(dim,(i+m),j)];

  /* różne zmienne pomocnicze */ 
  _LA_lwork = 1;
  _LA_tau = malloc(sizeof(FLOAT) * k);
  _LA_lwork = -1;
  _LA_permut = malloc(sizeof(long int) * k);
  L = malloc(sizeof(FLOAT) * k * k); /* macierz L z rozkładu LQ potrzebna do równania */

  /* liczymy rozkład LQ */
  GELQF(&k, &n, LQ, &k, _LA_tau, &buf, &_LA_lwork, &info);
  CHECK_INFO(info, "Workspace query for C matrix");
  _LA_lwork = lround(buf);
  _LA_work = malloc(sizeof(FLOAT) * _LA_lwork);

  GELQF(&k, &n, LQ, &k, _LA_tau, _LA_work, &_LA_lwork, &info);
  CHECK_INFO(info, "LQ factorization for C matrix");

  /* rozwiązujemy Ly_1 = e */
  memset(y, 0, sizeof(FLOAT) * n);
  memcpy(y, e, sizeof(FLOAT) * k);
  for(i = 0; i < k; i++)
    for(j = 0; j < k; j++)
      L[IND(k,i,j)] = i >= j ? LQ[IND(k,i,j)] : 0;
  GESV(&k, &_LA_nrhs, L,  &k, _LA_permut, y, &k, &info);
  CHECK_INFO(info, "Solving Ly_1 = e"); 

  /* liczymy B * Q'*/
  free(_LA_work);
  _LA_side = 'R';
  _LA_trans = 'T';
  _LA_lwork = -1;
  ORMLQ(&_LA_side, &_LA_trans, &m, &n, &k, LQ, &k, _LA_tau, B, &m, &buf, &_LA_lwork, &info);
  CHECK_INFO(info, "Workspace query for B matrix");
  _LA_lwork = lround(buf); 
  _LA_work = malloc(sizeof(FLOAT) * _LA_lwork);

  ORMLQ(&_LA_side, &_LA_trans, &m, &n, &k, LQ, &k, _LA_tau, B, &m, _LA_work, &_LA_lwork, &info);
  CHECK_INFO(info, "Calculating B * Q'");

  /* liczymy d - (B * Q_1) * y_1 */
  for(i = 0; i < m; i++)
  {
    sum = 0.0;
    for(j = 0; j < k; j++)
      sum += B[IND(m,i,j)] * y[j];
    d[i] -= sum;
  }

  /* liczymy LZMA dla B * Q_2' = d - (B * Q_1') * y_1 */
  _LA_nrhs = 1;
  dim = n - k;
  _LA_jpvt = malloc(sizeof(long int) * dim);
  memset(_LA_jpvt, 0, sizeof(long int) * dim);
  _LA_lwork = -1;
  GELSY(&m, &dim, &_LA_nrhs, &B[m * k], &m, d, &m, _LA_jpvt, &_LA_rcond, &_LA_rank, &buf, &_LA_lwork, &info);
  CHECK_INFO(info, "Least squares workspace query");  

  _LA_lwork = lround(buf);
  free(_LA_work);
  _LA_work = malloc(sizeof(FLOAT) * _LA_lwork); 
  GELSY(&m, &dim, &_LA_nrhs, &B[m * k], &m, d, &m, _LA_jpvt, &_LA_rcond, &_LA_rank, _LA_work, &_LA_lwork, &info);
  CHECK_INFO(info, "Computing least squares");
  memcpy(y + k, d, sizeof(FLOAT) * dim); 

  /* liczymy x = Q' * y */
  _LA_side = 'L';
  _LA_trans = 'T';
  _LA_nrhs = 1;
  _LA_lwork = -1;
  ORMLQ(&_LA_side, &_LA_trans, &n, &_LA_nrhs, &k, LQ, &k, _LA_tau, y, &n, &buf, &_LA_lwork, &info);
  CHECK_INFO(info, "x = Q' * y workspace query");

  _LA_lwork = lround(buf);
  free(_LA_work);
  _LA_work = malloc(sizeof(FLOAT) * _LA_lwork);
  ORMLQ(&_LA_side, &_LA_trans, &n, &_LA_nrhs, &k, LQ, &k, _LA_tau, y, &n, _LA_work, &_LA_lwork, &info);
  CHECK_INFO(info, "Computing x = Q' * y");

  printf("--\nans:\n");
  wypisz_macierz(n, 1, y);


  free(B);
  free(L);
  free(_LA_tau);
  free(_LA_work);
  free(_LA_permut);
  free(_LA_jpvt);
  return ;
}

int main()
{
  long int m, n, k;
  FLOAT *A, *d, *e, *x;
  wczytywanie(&m, &k, &n, &A, &d, &e);


  x = malloc(sizeof(FLOAT) * n);
  policz(m, k, n, A, d, e, x);

  free(A);
  free(d);
  free(e);
  free(x);

  return 0;
}
