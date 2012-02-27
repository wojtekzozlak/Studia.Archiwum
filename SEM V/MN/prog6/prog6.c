
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* ////////////////////////////////////////////////////////////////////////// */
double QuadSimpsond ( double a, double b, int n,
                      double (*f)(void *usrptr, double x), void *usrptr )
{
  int    i;
  double d, h, s;

  d = b-a;
  h = d/(double)n;
  s = f( usrptr, a ) + 4.0*f( usrptr, a+0.5*h );
  for ( i = 1; i < n; i++ )
    s += 2.0*f( usrptr, a+(double)i*h ) + 4.0*f( usrptr, a+((double)i+0.5)*h );
  s += f ( usrptr, b );
  return s*h/6.0;
} /*QuadSimpsond*/

/* ////////////////////////////////////////////////////////////////////////// */
double QuadGaussLegendre4d ( double a, double b, int n,
                             double (*f)(void *usrptr, double x), void *usrptr )
{
  int    i;
  double d, h, k1, k2, s;

  d = b-a;
  h = d/(double)n;
  s = 0.5 / sqrt ( 3.0 );
  k1 = 0.5-s;
  k2 = 0.5+s;
  s = 0.0;
  for ( i = 0; i < n; i++ ) {
    s += f ( usrptr, a + h*((double)i+k1) );
    s += f ( usrptr, a + h*((double)i+k2) );
  }
  return s*h/2.0;
} /*QuadGaussLegendre4d*/

double QuadGaussLegendre6d ( double a, double b, int n,
                             double (*f)(void *usrptr, double x), void *usrptr )
{
  int    i, j;
  double kn[3] = {-0.7745966692414834, 0.0, 0.7745966692414834};
  double qc[3] = {5.0/9.0, 8.0/9.0, 5.0/9.0};
  double d, h, fv, s;

  d = b-a;
  h = d/(double)n;
  s = 0.0;
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < 3; j++ ) {
      fv = f ( usrptr, a + h*((double)i+0.5+0.5*kn[j]) );
      s += fv*qc[j];
    }
  return s*h/2.0;
} /*QuadGaussLegendre6d*/

double QuadGaussLegendre8d ( double a, double b, int n,
                             double (*f)(void *usrptr, double x), void *usrptr )
{
  int    i, j;
  double kn[4] = {-0.8611363115940526, -0.3399810435848563,
                   0.3399810435848563,  0.8611363115940526};
  double qc[4] = {0.3478548451374539, 0.6521451548625461,
                  0.6521451548625461, 0.3478548451374539};
  double d, h, fv, s;

  d = b-a;
  h = d/(double)n;
  s = 0.0;
  for ( i = 0; i < n; i++ )
    for ( j = 0; j < 4; j++ ) {
      fv = f ( usrptr, a + h*((double)i+0.5+0.5*kn[j]) );
      s += fv*qc[j];
    }
  return s*h/2.0;
} /*QuadGaussLegendre8d*/

/* ////////////////////////////////////////////////////////////////////////// */
double f ( void *usrptr, double x )
{
/*
  if ( x )
    return sin ( x )/x;
  else
    return 1.0;
*/
  if ( x < 0.125 )
    return 0;
  else {
    x -= 0.125;
    return x * x * x * x * x * x * x;
  }
} /*f*/

int main ( void )
{
  printf ( "s, 1  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 1, f, NULL ) );
  printf ( "s, 2  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 2, f, NULL ) );
  printf ( "s, 4  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 4, f, NULL ) );
  printf ( "s, 8  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 8, f, NULL ) );
  printf ( "s,16  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 16, f, NULL ) );
  printf ( "s,32  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 32, f, NULL ) );
  printf ( "s,64  = %12.9f\n", QuadSimpsond ( 0.0, 1.0, 64, f, NULL ) );
  printf ( "q4, 1 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 1, f, NULL ) );
  printf ( "q4, 2 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 2, f, NULL ) );
  printf ( "q4, 4 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 4, f, NULL ) );
  printf ( "q4, 8 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 8, f, NULL ) );
  printf ( "q4,16 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 16, f, NULL ) );
  printf ( "q4,32 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 32, f, NULL ) );
  printf ( "q4,64 = %12.9f\n", QuadGaussLegendre4d ( 0.0, 1.0, 64, f, NULL ) );
  printf ( "q6, 1 = %12.9f\n", QuadGaussLegendre6d ( 0.0, 1.0, 1, f, NULL ) );
  printf ( "q6, 2 = %12.9f\n", QuadGaussLegendre6d ( 0.0, 1.0, 2, f, NULL ) );
  printf ( "q6, 4 = %12.9f\n", QuadGaussLegendre6d ( 0.0, 1.0, 4, f, NULL ) );
  printf ( "q6, 8 = %12.9f\n", QuadGaussLegendre6d ( 0.0, 1.0, 8, f, NULL ) );
  printf ( "q8, 1 = %12.9f\n", QuadGaussLegendre8d ( 0.0, 1.0, 1, f, NULL ) );
  printf ( "q8, 2 = %12.9f\n", QuadGaussLegendre8d ( 0.0, 1.0, 2, f, NULL ) );
  printf ( "q8, 4 = %12.9f\n", QuadGaussLegendre8d ( 0.0, 1.0, 4, f, NULL ) );
  printf ( "q8, 8 = %12.9f\n", QuadGaussLegendre8d ( 0.0, 1.0, 8, f, NULL ) );
  exit ( 0 );
} /*main*/

