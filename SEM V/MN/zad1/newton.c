#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "f2c.h"
#include "clapack.h"
#include "newton.h"

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
    /* oblicz wartosc funkcji i pochodna */
    fd ( n, usrptr, x, f, Df );

    /* residualne kryterium stopu */
    for ( res = 0.0, j = 0;  j < n;  j++ )
      res += f[j]*f[j];
    if ( res <= delta*delta )
      goto sukces;

    /* rozwiaz uklad rownan Df*dx = -f i oblicz nowy punkt */
    info = 0;
    GESV ( &n, &one, Df, &n, permut, f, &n, &info );
    if ( info ) {
/*      printf ( "GESV info: %ld\n", info ); */
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

  /* maxit */
  return -1;

klops:
  free ( f );
  return 0;

sukces:
  free ( f );
  return 1;
} /*SysNewton*/
