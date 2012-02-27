
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "f2c.h"
#include "clapack.h"

/*#define __DOUBLE__*/
#ifndef __DOUBLE__
#define FLOAT float
#define GESV sgesv_
#else
#define FLOAT double
#define GESV dgesv_
#endif

#define IND(n,i,j) ((n)*(j)+(i))

#define EKSPERYMENT

char SysNewton ( long int n,
                 void (*fd)(long int n, void *usrptr,
                            FLOAT *x, FLOAT *f, FLOAT *Df),
                 void *usrptr, FLOAT eps, FLOAT delta, int maxit,
                 FLOAT *x )
{
  int   i, j;
  FLOAT *f, *Df, res, dx;
  long int   *permut, one = 1, info;

  f = malloc ( n*(n+1)*sizeof(FLOAT) + n*sizeof(long int) );
  if ( !f )
    return 0;
  Df = &f[n];
  permut = (long int*)&Df[n*n];
  for ( i = 0; i < maxit; i++ ) {
#ifdef EKSPERYMENT
    printf ( "%2d: ", i );
    for ( j = 0; j < n; j++ )
      printf ( "%12.9f,", x[j] );
/*    printf ( "\n" );*/
#endif
        /* oblicz wartosc funkcji i pochodna */
    fd ( n, usrptr, x, f, Df );
#ifdef EKSPERYMENT
    printf ( "   " );
    for ( j = 0; j < n; j++ )
      printf ( "%12.9f,", f[j] );
    printf ( "\n" );
#endif
        /* residualne kryterium stopu */
    for ( res = 0.0, j = 0;  j < n;  j++ )
      res += f[j]*f[j];
    if ( res <= delta*delta )
      goto sukces;
        /* rozwiaz uklad rownan Df*dx = -f i oblicz nowy punkt */
    info = 0;
    GESV ( &n, &one, Df, &n, permut, f, &n, &info );
    if ( info ) {
      printf ( "GESV info: %ld\n", info );
      goto klops;
    }
    for ( j = 0; j < n; j++ )
      x[j] -= f[j];
        /* przyrostowe kryterium stopu */
    for ( dx = 0.0, j = 0;  j < n;  j++ )
      dx += f[j]*f[j];
    if ( dx < eps*eps )
      goto sukces;
  }

klops:
  free ( f );
  return 0;

sukces:
  free ( f );
  return 1;
} /*SysNewton*/

#define NN 3
void TestFD ( long int n, void *usrptr, FLOAT *x, FLOAT *f, FLOAT *Df )
{
        /* n == NN == 3 */
  f[0] = x[0] + x[1] + x[2] - 3.0;
  f[1] = x[0]*x[0] + x[1]*x[1] + x[2]*x[2] - 17.0;
  f[2] = x[0]*x[1]*x[2] + 12.0;
  Df[IND(NN,0,0)] = 1.0;
  Df[IND(NN,0,1)] = 1.0;
  Df[IND(NN,0,2)] = 1.0;
  Df[IND(NN,1,0)] = 2.0*x[0];
  Df[IND(NN,1,1)] = 2.0*x[1];
  Df[IND(NN,1,2)] = 2.0*x[2];
  Df[IND(NN,2,0)] = x[1]*x[2];
  Df[IND(NN,2,1)] = x[0]*x[2];
  Df[IND(NN,2,2)] = x[0]*x[1];
} /*TestFD*/

void TestSysNewton ( void )
{
  FLOAT x[NN] = { 1.0, 2.0, -2.0 };
/* inny punkt startowy */
/*  FLOAT x[NN] = { 1.0, 2.0, -1.0 }; */
  int   i;

  printf ( "Test metody Newtona\n" );
  if ( SysNewton ( NN, TestFD, NULL, 1.0e-6, 1.0e-6, 20, x ) ) {
    for ( i = 0; i < NN; i++ )
      printf ( "%f,", x[i] );
    printf ( "\n" );
  }
  else
    printf ( "klops\n" );
} /*TestSysNewton*/
#undef NN

int main ( void )
{
  TestSysNewton ();
  exit ( 0 );
} /*main*/

