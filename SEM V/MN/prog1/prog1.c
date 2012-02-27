
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define FLOAT float

FLOAT f1 ( void *usrptr, FLOAT x )
{
  return sin ( x ) - 0.5*x;
} /*f1*/

void fd1 ( void *usrptr, FLOAT x, FLOAT *f, FLOAT *df )
{
  *f = sin ( x ) - 0.5*x;
  *df = cos ( x ) - 0.5;
  return;
} /*fd1*/

void Bisekcja ( FLOAT (*f)(void *usrptr, FLOAT x), void *usrptr,
                FLOAT a, FLOAT b, FLOAT eps,
                FLOAT *x, char *error )
{
  FLOAT c, fa, fb, fc;

  fa = f ( usrptr, a );
  fb = f ( usrptr, b );
  if ( fa*fb <= 0.0 && eps > 0.0 ) {
    while ( fabs(b-a) > eps ) {
      c = 0.5*(a+b);
      fc = f ( usrptr, c );
      if ( fa*fc <= 0.0 ) b = c;
                     else a = c;
    }
    *x = a;
    *error = 0;
  }
  else
    *error = 1;
} /*Bisekcja*/

void Newton ( void (*fd)(void *usrptr, FLOAT x, FLOAT *f, FLOAT *fd), void *usrptr,
              FLOAT x0, FLOAT eps, FLOAT delta, int maxit,
              FLOAT *x, char *error )
{
  FLOAT f0, d0, dx;
  int   i;

  if ( eps > 0.0 || delta > 0.0 ) {
    for ( i = 0; i < maxit; i++ ) {
      fd ( usrptr, x0, &f0, &d0 );
      dx = f0/d0;
      if ( fabs (f0) < delta || fabs(dx) < eps ) {
        *x = x0;
        *error = 0;
        return;
      }
      x0 -= dx;
    }
  }
  *error = 1;
} /*Newton*/

void Secant ( FLOAT (*f)(void *usrptr, FLOAT x), void *usrptr,
              FLOAT a, FLOAT b, FLOAT eps, FLOAT delta, int maxit,
              FLOAT *x, char *error )
{
  FLOAT c, fa, fb, fc;
  int   i;

  if ( eps > 0.0 || delta > 0.0 ) {
    fa = f ( usrptr, a );
    fb = f ( usrptr, b );
    for ( i = 0; i < maxit; i++ ) {
      c = a - (b-a)/(fb-fa)*fa;
      fc = f ( usrptr, c );
      if ( fabs(fc) < delta || fabs(c-b) < eps ) {
        *x = c;
        *error = 0;
        return;
      }
      a = b;  fa = fb;
      b = c;  fb = fc;
    }
  }
  *error = 1;
} /*Secant*/

int main ( void )
{
  FLOAT x;
  char  error;

  Bisekcja ( f1, NULL, 1.0, 3.0, 1.0e-6, &x, &error );
  if ( error )
    printf ( "Blad\n" );
  else
    printf ( "rozwiazanie x = %f\n", x );
  Newton ( fd1, NULL, 1.0, 1.0e-6, 1.0e-6, 20, &x, &error );
  if ( error )
    printf ( "Blad\n" );
  else
    printf ( "rozwiazanie x = %f\n", x );
  Secant ( f1, NULL, 1.0, 3.0, 1.0e-6, 1.0e-6, 20, &x, &error );
  if ( error )
    printf ( "Blad\n" );
  else
    printf ( "rozwiazanie x = %f\n", x );
  exit ( 0 );
} /*main*/

