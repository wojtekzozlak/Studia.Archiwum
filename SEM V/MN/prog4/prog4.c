
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define FLOAT double

typedef struct {
    int   i, j;
    FLOAT aij;
  } spmelem;

/* ///////////////////////////////////////////////////////////////////////// */
/* mnozenie macierzy spakowanej przez wektor */
void MultSPMatV ( int n, int nnze, spmelem *mat, FLOAT *x, FLOAT *y )
{
  int k;

  memset ( y, 0, n*sizeof(FLOAT) );
  for ( k = 0; k < nnze; k++ )
    y[mat[k].i] += mat[k].aij*x[mat[k].j];
} /*MultSPMatV*/

FLOAT NormaResiduum ( int n, int nnze, spmelem *mat, FLOAT *b, FLOAT *x )
{
  FLOAT *y, res;
  int   i;

  y = malloc ( n*sizeof(FLOAT) );
  if ( !y )
    exit ( 1 );   /* !!! */
  MultSPMatV ( n, nnze, mat, x, y );
  for ( res = 0.0, i = 0;  i < n;  i++ ) {
    y[i] -= b[i];
    res += y[i]*y[i];
  }
  free ( y );
  res = sqrt ( res );
printf ( "%f\n", res );
  return res;
} /*NormaResiduum*/

void Jacobi ( int n, int nnze, spmelem *mat,
              FLOAT *b, FLOAT *x, FLOAT eps )
{
  FLOAT *y, r0;
  int   k, i, j, iter;

  y = malloc ( n*sizeof(FLOAT) );
  if ( !y )
    return;
  r0 = NormaResiduum ( n, nnze, mat, b, x );
iter = 0;
  do {
iter ++;
    memcpy ( y, b, n*sizeof(FLOAT) );
    for ( k = 0; k < nnze; k++ ) {
      i = mat[k].i;
      j = mat[k].j;
      if ( i != j )
        y[i] -= mat[k].aij*x[j];
    }
    for ( k = 0; k < nnze; k++ ) {
      i = mat[k].i;
      if ( i == mat[k].j )
        x[i] = y[i]/mat[k].aij;
    }
  } while ( NormaResiduum ( n, nnze, mat, b, x ) > eps*r0 );
  free ( y );
printf ( "wykonanych %d iteracji\n", iter );
} /*Jacobi*/

void GaussSeidel ( int n, int nnze, spmelem *mat,
                   FLOAT *b, FLOAT *x, FLOAT eps )
{
} /*GaussSeidel*/

void Richardson ( int n, int nnze, spmelem *mat, FLOAT tau,
                  FLOAT *b, FLOAT *x, FLOAT eps )
{
} /*Richardson*/

/* ///////////////////////////////////////////////////////////////////////// */
static void rQuickSort ( int i, int j, void *usrptr,
                         char (*less)(int,int,void*),
                         void (*swap)(int,int,void*) )
{
  int p, q;

  while ( j-i > 8 ) {
        /* finding the pivot */
    p = (i+j)/2;
        /* partition */
    swap ( i, p, usrptr );
    p = i;
    for ( q = i+1; q <= j; q++ )
      if ( less ( q, i, usrptr ) )
        swap ( ++p, q, usrptr );
    swap ( i, p, usrptr );
        /* one recursive call, followed by repetition in the while loop. */
        /* the if instruction ensures the smallest recursion depth. */
    if ( j-p < p-i ) {
      rQuickSort ( p+1, j, usrptr, less, swap );
      j = p-1;
    }
    else {
      rQuickSort ( i, p-1, usrptr, less, swap );
      i = p+1;
    }
  }
} /*rQuickSort*/

void QuickSort ( int n, void *usrptr,
                 char(*less)(int,int,void*),
                 void (*swap)(int,int,void*) )
{
  int i, j, k;

  if ( n > 1 ) {
        /* use the true QuickSort first */
    rQuickSort ( 0, n-1, usrptr, less, swap );
        /* then finish the work by using InsertionSort */
    for ( i = 1; i < n; i++ ) {
      k = i;  j = k-1;
      while ( j >= 0 )
        if ( less ( k, j, usrptr ) ) {
          swap ( j, k, usrptr );
          k = j--;
        }
        else break;
    }
  }
} /*QuickSort*/

/* ///////////////////////////////////////////////////////////////////////// */
char my_less ( int i, int j, void *usrptr )
{
  spmelem *a;

  a = (spmelem*)usrptr;
  if ( a[i].i < a[j].i )
    return 1;
  else if ( a[i].i == a[j].i && a[i].j < a[j].j )
    return 1;
  else
    return 0;
} /*my_less*/

void my_swap ( int i, int j, void *usrptr )
{
  spmelem *a, b;

  a = (spmelem*)usrptr;
  b = a[i];  a[i] = a[j];  a[j] = b;
} /*my_swap*/

void SetupLapMat ( int M, int N, int *n, int *nnze, spmelem **mat, FLOAT *hh )
{
  int     _n, _nnze, i, j, k;
  spmelem *_mat;
  FLOAT   hh1, hh2;

  *n = _n = N*M;
  *nnze = _nnze = 5*M*N - 2*(M+N);
  *mat = _mat = malloc ( _nnze*sizeof(spmelem) );
  if ( !_mat )
    return;
  memset ( _mat, 0, _nnze*sizeof(spmelem) );
      /* okreslanie niezerowych elementow */
  hh1 = (FLOAT)(N+1);  hh1 *= hh1;
  hh2 = (FLOAT)(M+1);  hh2 *= hh2;
  *hh = hh1*hh2;
      /* na diagonali */
  for ( k = 0; k < M*N; k++ ) {
    _mat[k].i = _mat[k].j = k;
    _mat[k].aij = 2.0*(hh1+hh2);
  }
      /* na kodiagonalach */
  for ( i = 0; i < N; i++ )
    for ( j = 0;  j < M-1;  j++, k += 2 ) {
      _mat[k].i = _mat[k+1].j = i*M+j+1;
      _mat[k].j = _mat[k+1].i = i*M+j;
      _mat[k].aij = _mat[k+1].aij = -hh1;
    }
      /* na diagonalach blokow kodiagonalnych */
  for ( i = 0; i < N-1; i++ )
    for ( j = 0;  j < M;  j++, k += 2 ) {
      _mat[k].i = _mat[k+1].j = i*M+j;
      _mat[k].j = _mat[k+1].i = (i+1)*M+j;
      _mat[k].aij = _mat[k+1].aij = -hh2;
    }
      /* posortuj */
  QuickSort ( _nnze, _mat, my_less, my_swap );
} /*SetupLapMat*/

void Test1 ( int M, int N )
{
#define EPS 1.0e-4
  int     i, n, nnze;
  spmelem *mat;
  FLOAT   hh, *x, *b;

  SetupLapMat ( M, N, &n, &nnze, &mat, &hh );
  if ( !mat )
    exit ( 1 );
  x = malloc ( 2*n*sizeof(FLOAT) );
  if ( !x )
    exit ( 1 );
  b = &x[n];
  for ( i = 0; i < n; i++ )
    b[i] = hh;
        /* teraz rozne metody iteracyjne rozwiazywania ukladow rownan liniowych */
        /* startujemy od zera */
  printf ( "Metoda Jacobiego:\n" );
  memset ( x, 0, n*sizeof(FLOAT) );
  Jacobi ( n, nnze, mat, b, x, EPS );

  printf ( "Metoda Gaussa-Seidela:\n" );
  memset ( x, 0, n*sizeof(FLOAT) );
  GaussSeidel ( n, nnze, mat, b, x, EPS );

  printf ( "Metoda Richardsona:\n" );
  memset ( x, 0, n*sizeof(FLOAT) );
  Richardson ( n, nnze, mat, 0.01, b, x, EPS );

  free ( mat );
  free ( x );
#undef EPS
} /*Test1*/

int main ( void )
{
  Test1 ( 20, 20 );
  exit ( 0 );
} /*main*/

