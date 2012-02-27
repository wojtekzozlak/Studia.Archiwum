
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define FLOAT float

#define IND(n,i,j) ((n)*(i)+(j))

char LUDecomp ( int n, FLOAT *a, int *permut )
{
  int   i, j, k;
  FLOAT m, mmax;

        /* rozklad metoda eliminacji Gaussa */
  for ( j = 0; j < n; j++ ) {
          /* wybor elementu glownego */
    mmax = fabs(a[IND(n,j,j)]);
    k = j;
    for ( i = j+1; i < n; i++ ) {
      m = fabs(a[IND(n,i,j)]);
      if ( m > mmax ) {
        mmax = m;
        k = i;
      }
    }
          /* przestawianie wierszy */
    if ( (permut[j] = k) != j )
      for ( i = 0; i < n; i++ )
        { m = a[IND(n,j,i)];  a[IND(n,j,i)] = a[IND(n,k,i)];  a[IND(n,k,i)] = m; }
          /* wlasciwa eliminacja Gaussa */
    if ( a[IND(n,j,j)] == 0.0 )
      return 0;
    for ( i = j+1; i < n; i++ ) {
      m = a[IND(n,i,j)] = a[IND(n,i,j)]/a[IND(n,j,j)];
      for ( k = j+1; k < n; k++ )
        a[IND(n,i,k)] -= a[IND(n,j,k)]*m;
    }
  }
  return 1;
} /*LUDecomp*/

void LUSolve ( int n, FLOAT *a, int *permut, FLOAT *b )
{
  int   i, j;
  FLOAT m;

        /* przestawianie wspolczynnikow prawej strony */
  for ( i = 0; i < n-1; i++ )
    if ( (j = permut[i]) != i )
      { m = b[i];  b[i] = b[j];  b[j] = m; }
        /* rozwiazywanie ukladu L*y = b */
  for ( i = 1; i < n; i++ )
    for ( j = 0; j < i; j++ )
      b[i] -= a[IND(n,i,j)]*b[j];
        /* rozwiazywanie ukladu U*x = y */
  for ( i = n-1; i >= 0; i-- ) {
    for ( j = i+1; j < n; j++ )
      b[i] -= a[IND(n,i,j)]*b[j];
    b[i] /= a[IND(n,i,i)];
  }
} /*LUSolve*/

FLOAT detLU ( int n, FLOAT *a, char *error )
{
  int   *permut, i;
  FLOAT det;
  char  chs;

  *error = 1;
  permut = (int*)malloc ( n*sizeof(int) );
  if ( permut ) {
    if ( LUDecomp ( n, a, permut ) ) {
      for ( det = a[IND(n,0,0)], i = 1;  i < n;  i++ )
        det *= a[IND(n,i,i)];
      for ( chs = 0, i = 0;  i < n;  i++ )
        if ( permut[i] != i )
          chs = !chs;
      if ( chs )
        det = -det;
      *error = 0;
    }
    else
      det = 0.0;
    free ( permut );
    return det;
  }
  else return 0.0;
} /*detLU*/

FLOAT r_detLaplace ( int n, FLOAT **a, char *error )
{
  FLOAT **s, det, dd;
  int   i;

  if ( n == 1 ) {
    *error = 0;
    return a[0][0];
  }
  else {
    s = (FLOAT**)malloc ( (n-1)*sizeof(FLOAT*) );
    if ( s ) {
      for ( i = 1; i < n; i++ )
        s[i-1] = &a[i][1];
      det = 0.0;
      for ( i = 0; i < n; i++ ) {
        dd = r_detLaplace ( n-1, s, error );
        if ( *error )
          goto way_out;
        if ( i & 0x01 )
          det -= a[i][0]*dd;
        else
          det += a[i][0]*dd;
        if ( i < n-1 )
          s[i] = &a[i][1];
      }
way_out:
      free ( s );
      return det;
    }
    else {
      *error = 1;
      return 0.0;
    }
  }
} /*r_detLaplace*/

FLOAT detLaplace ( int n, FLOAT *a, char *error )
{
  FLOAT **r, det;
  int   i;

  r = (FLOAT**)malloc ( n*sizeof(FLOAT*) );
  if ( r ) {
    for ( i = 0; i < n; i++ )  /* UWAGA, trik! */
      r[i] = &a[n*i];
    det = r_detLaplace ( n, r, error );
    free ( r );
    return det;
  }
  else {
    *error = 1;
    return 0.0;
  }
} /*detLaplace*/

void SetTestMatrix ( int n, FLOAT *a )
{
  int   i, j;
  FLOAT c;

  for ( i = 0; i < n; i++ ) {
    c = a[IND(n,i,i)] = 1.0;
    for ( j = i+1; j < n; j++ ) {
      c *= 0.25;
      a[IND(n,i,j)] = a[IND(n,j,i)] = c;
    }
  }
} /*SetTestMatrix*/

void SetHilbertMatrix ( int n, FLOAT *a )
{
  int i, j;

  for ( i = 0; i < n; i++ )
    for ( j = 0; j < n; j++ )
      a[IND(n,i,j)] = 1.0/(FLOAT)(1+i+j);
} /*SetHilbertMatrix*/

void TestDet ( void )
{
#define N 4
  FLOAT a[N*N], b[N*N];
  FLOAT det0, det1;
  char  error;

  SetTestMatrix ( N, a );
/*  SetHilbertMatrix ( N, a ); */
  memcpy ( b, a, N*N*sizeof(FLOAT) );
  printf ( "n = %d\n", N );
  printf ( "obliczenie wyznacznika za pomoca eliminacji\n" );
  det0 = detLU ( N, a, &error );
  if ( error )
    printf ( "  Blad!\n" );
  else
    printf ( "  det A = %e\n", det0 );
  printf ( "obliczenie wyznacznika za pomoca rozwiniecia Laplace'a:\n" );
  det1 = detLaplace ( N, b, &error );
  if ( error )
    printf ( "  Blad!\n" );
  else
    printf ( "  det A = %e\n", det1 );
} /*TestDet*/

int main ( void )
{
  TestDet ();
  exit ( 0 );
} /*main*/

