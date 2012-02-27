#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "f2c.h"
#include "clapack.h"

#include "postscript.h"


#define __DOUBLE__
#ifndef __DOUBLE__
#define FLOAT float
#define GTTRF sgttrf_
#define GTTRS sgttrs_
#else
#define FLOAT double
#define GTTRF dgttrf_
#define GTTRS dgttrs_
#endif

#define NN 200  /* liczba odcinkow */

struct {
    FLOAT ax, bx, ay, by;
  } map;

/* ////////////////////////////////////////////////////////////////// */
#define NMAX 20
typedef struct {
    int n;
    FLOAT x[NMAX+1];
    FLOAT a[NMAX+1];
  } wielomian;

FLOAT Aitken ( void *usrptr, FLOAT x )
{
  wielomian w;
  int i, k;

  memcpy ( &w, usrptr, sizeof(wielomian) );
  for ( k = 1; k <= w.n; k++ )
    for ( i = 0; i <= w.n-k; i++ )
      w.a[i] = ((w.x[i+k]-x)*w.a[i] +
                (x-w.x[i])*w.a[i+1])/(w.x[i+k]-w.x[i]);
  return w.a[0];
} /*Aitken*/

void Lagrange ( wielomian *w, wielomian *h )
{
  int i, j;

  memcpy ( h, w, sizeof(wielomian) );
  for ( j = 1; j <= h->n; j++ )
    for ( i = h->n; i >= j; i-- )
      h->a[i] = (h->a[i]-h->a[i-1])/(h->x[i]-h->x[i-j]);
} /*Lagrange*/

FLOAT npoly ( void *usrptr, FLOAT x )
{
  int       i, n;
  wielomian *h;
  FLOAT     p;

  h = (wielomian*)usrptr;
  n = h->n;
  p = h->a[n];
  for ( i = n-1; i >= 0; i-- )
    p = p*(x-h->x[i]) + h->a[i];
  return p;
} /*npoly*/

/* ////////////////////////////////////////////////////////////////// */
#define NSM 100  /* maksymalna liczba lukow wielomianowych */

typedef struct {
    int N;
    FLOAT u[NSM+1];  /* wezly */
    FLOAT a[NSM+1];  /* wartosci funkcji */
    FLOAT b[NSM+1];  /* wartosci pochodnej */
    FLOAT c[NSM];    /* roznice dzielone rzedu 2 */
    FLOAT d[NSM];    /* roznice dzielone rzedu 3 */
  } spline3;

char ConstructSpline ( spline3 *s )
{
  FLOAT    *dl, *d, *du, *duu, *r, h;
  long int *permut;
  long int info, n, one = 1;
  char     trans = 'N';
  int      i;

  n = s->N+1;  /* liczba rownan i niewiadomych */
  if ( n < 4 )
    return 0;
  dl = (FLOAT*)malloc ( 5*n*sizeof(FLOAT)+n*sizeof(long int) );
  if ( !dl )      /* pierwsza pod diagonala */
    return 0;
  d = &dl[n];   /* diagonala */
  du = &d[n];     /* pierwsza nad diagonala */
  duu = &du[n]; /* druga nad diagonala */
  r = &duu[n];  /* roznice */
  permut = (long int*)&r[n];
        /* oblicz dlugosci przedzialow */
  for ( i = 0; i < n-1; i++ ) {
    duu[i] = s->u[i+1] - s->u[i];
    if ( duu[i] <= 0.0 )   /* sprawdzenie poprawnosci ciagu wezlow */
      goto klops;
    r[i] = s->a[i+1] - s->a[i];
  }
        /* utworz uklad rownan - naturalna kubiczna krzywa sklejana */
  d[0] = 2.0*duu[0];  du[0] = duu[0];
  s->b[0] = 3.0*r[0];
  for ( i = 1; i < n-1; i++ ) {
    dl[i-1] = duu[i];  d[i] = 2.0*(duu[i-1]+duu[i]);  du[i] = duu[i-1];
    s->b[i] = 3.0*(du[i]/du[i-1]*r[i-1] + du[i-1]/du[i]*r[i]);
  }
  dl[n-2] = duu[n-2];  d[n-1] = 2.0*duu[n-2];
  s->b[n-1] = 3.0*r[n-2];
        /* rozwiaz uklad rownan z macierza trojdiagonalna */
  info = 0;
  GTTRF ( &n, dl, d, du, duu, permut, &info );
  if ( info )
    goto klops;
  GTTRS ( &trans, &n, &one, dl, d, du, duu, permut, s->b, &n, &info );
  if ( info )
    goto klops;
        /* oblicz roznice dzielone rzedu 2 i 3 */
  for ( i = 0; i < n-1; i++ ) {
    h = s->u[i+1] - s->u[i];
    s->c[i] = (s->a[i+1] - s->a[i]) / h;
    s->d[i] = (s->b[i+1] - s->c[i]) / h;
    s->c[i] = (s->c[i] - s->b[i]) / h;
    s->d[i] = (s->d[i] - s->c[i]) / h;
  }

  free ( dl );
  return 1;

klops:
  free ( dl );
  return 0;
} /*ConstructSpline*/

FLOAT EvalSpline ( void *usrptr, FLOAT x )
{
  spline3 *s;
  int     n, k, i;
  FLOAT   w;

  s = (spline3*)usrptr;
        /* wyszukiwanie przedzialu - binarne */
  n = s->N;
  k = 0;
  do {
    i = (n+k)/2;
    (x < s->u[i]) ? (n = i) : (k = i);
  } while ( n-k > 1 );
        /* schemat Hornera dla bazy Newtona */
  w = ((s->d[k]*(x-s->u[k+1])+s->c[k])*(x-s->u[k])+s->b[k])*(x-s->u[k])+s->a[k];
  return w;
} /*EvalSpline*/

/* ////////////////////////////////////////////////////////////////// */
#define MAXBSDEG     5
#define MAXBSKNOTS 200

typedef struct {
    int N, deg;
    FLOAT u[MAXBSKNOTS];
    FLOAT d[MAXBSKNOTS-1];
  } BSpline;

void deBoor ( int deg, int lkn, FLOAT *u, FLOAT x, int *k, FLOAT *b )
{
        /* algorytm de Boora obliczania wartosci funkcji B-sklejanych */
  int   _k, i, j, n, l;
  FLOAT alpha, beta;

        /* wyszukiwanie przedzialu - binarne */
  n = lkn-deg;
  _k = deg;
  do {
    i = (n+_k)/2;
    (x < u[i]) ? (n = i) : (_k = i);
  } while ( n-_k > 1 );
  *k = _k;
        /* obliczanie wartosci wielomianow niezerowych w przedziale [u_k,u_{k+1}) */
  l = _k-deg;
  b[_k-l] = 1.0;
  for ( j = 1; j <= deg; j++ ) {
    beta = (u[_k+1]-x)/(u[_k+1]-u[_k-j+1]);
    b[_k-l-j] = beta*b[_k-l-j+1];
    for ( i = _k-j+1; i < _k; i++ ) {
      alpha = 1.0-beta;
      beta = (u[i+j+1]-x)/(u[i+j+1]-u[i+1]);
      b[i-l] = alpha*b[i-l] + beta*b[i-l+1];
    }
    b[_k-l] *= (1.0-beta);
  }
} /*deBoor*/

char ConstructInterpBSpline2 ( int nikn, FLOAT *v,
                               FLOAT (*f)(void*,FLOAT), void *usrptr,
                               BSpline *bs )
{
  int      i, k, N;
  FLOAT    *dl, *d, *du, *duu, *r, b[3];
  long int *permut, info, n, one = 1;
  char     trans = 'N';

  if ( nikn < 3 || nikn > MAXBSKNOTS-1 )
    return 0;  /* za malo lub za duzo wezlow */
        /* sprawdzamy, czy wezly interpolacyjne tworza ciag rosnacy */
  for ( i = 0; i < nikn-1; i++ )
    if ( v[i+1] <= v[i] )
      return 0;
        /* tworzymy ciag wezlow funkcji sklejanej */
  bs->deg = 2;
  bs->N = N = nikn+2;
  bs->u[0] = bs->u[1] = bs->u[2] = v[0];
  for ( i = 3; i < N-2; i++ )
    bs->u[i] = 0.5*(v[i-2]+v[i-1]);
  bs->u[N-2] = bs->u[N-1] = bs->u[N] = v[nikn-1];
        /* tworzymy uklad rownan dla funkcji interpolacyjnej */
  n = nikn;  /* liczba rownan */
  dl = malloc ( 4*n*sizeof(FLOAT)+n*sizeof(long int) );
  if ( !dl )      /* pierwsza pod diagonala */
    return 0;
  d = &dl[n];     /* diagonala */
  du = &d[n];     /* pierwsza nad diagonala */
  duu = &du[n];   /* druga nad diagonala */
  permut = (long int*)&duu[n];
          /* prawa strona - wartosci funkcji f */
  r = bs->d;
  for ( i = 0; i < nikn; i++ )
    r[i] = f ( usrptr, v[i] );
          /* macierz ukladu */
  d[0] = 1.0;   /* w tym wezle tylko jedna funkcja bazowa jest niezerowa */
  du[0] = 0.0;
  for ( i = 1; i < n-1; i++ ) {
                /* w tych wezlach sa niezerowe 3 funkcje B-sklejane */
    deBoor ( 2, N, bs->u, v[i], &k, b );
    dl[i-1] = b[0];
    d[i] = b[1];
    du[i] = b[2];
  }
                /* w tym wezle tez tylko jedna funkcja bazowa jest niezerowa */
  dl[n-2] = 0.0;
  d[n-1] = 1.0;
        /* rozwiazujemy uklad z macierza trojdiagonalna */
  info = 0;
  GTTRF ( &n, dl, d, du, duu, permut, &info );
  if ( info )
    goto klops;
  GTTRS ( &trans, &n, &one, dl, d, du, duu, permut, r, &n, &info );
  if ( info )
    goto klops;

  free ( dl );
  return 1;
 
klops:
  free ( dl );
  return 0;
} /*ConstructInterpBSpline2*/

FLOAT EvalBSpline ( void *usrptr, FLOAT x )
{
  BSpline *bs;
  int     i, j, k, l, n, deg;
  FLOAT   d[MAXBSDEG+1], alpha;
  FLOAT   *u;

  bs = (BSpline*)usrptr;
  deg = bs->deg;
  u = bs->u;
        /* wyszukiwanie przedzialu - binarne */
  k = deg;
  n = bs->N-deg;
  do {
    i = (n+k)/2;
    (x < u[i]) ? (n = i) : (k = i);
  } while ( n-k > 1 );
        /* algorytm de Boora */
  memcpy ( d, &bs->d[k-deg], (deg+1)*sizeof(FLOAT) );
  for ( j = 1; j <= deg; j++ )
    for ( i = k-deg+j, l = 0;  i <= k;  i++, l++ ) {
      alpha = (x-u[i])/(u[i+deg+1-j]-u[i]);
      d[l] = (1.0-alpha)*d[l] + alpha*d[l+1];
    }
  return d[0];
} /*EvalBSpline*/

/* ////////////////////////////////////////////////////////////////// */
FLOAT sinc ( void *usrptr, FLOAT x )
{
  if ( x == 0.0 )
    return 1.0;
  else
    return sin ( x ) / x;
} /*sinc*/

FLOAT ff ( void *usrptr, FLOAT x )
{
  return 1.0/(x*x+5.0);
} /*ff*/

/* ////////////////////////////////////////////////////////////////// */
void InitMapping ( FLOAT xmin, FLOAT xmax, FLOAT ymin, FLOAT ymax,
                   FLOAT ximin, FLOAT ximax, FLOAT etamin, FLOAT etamax )
{
  map.ax = (ximax-ximin)/(xmax-xmin);
  map.bx = ximin - map.ax*xmin;
  map.ay = (etamax-etamin)/(ymax-ymin);
  map.by = etamin - map.ay*ymin;
} /*InitMapping*/

FLOAT MapX ( FLOAT x )
{
  return map.ax*x + map.bx;
} /*MapX*/

FLOAT MapY ( FLOAT y )
{
  return map.ay*y + map.by;
} /*MapY*/

void DrawAxes ( FLOAT xmin, FLOAT xmax, FLOAT dx, FLOAT y0,
                FLOAT ymin, FLOAT ymax, FLOAT dy, FLOAT x0 )
{
  FLOAT xi0, xi1, xi, eta0, eta1, eta, x, y;

         /* rysuj os pozioma */
  xi0 = MapX ( xmin );
  xi1 = MapX ( xmax );
  eta = MapY ( y0 );
  PSSetLineWidth ( 3.0 );
  PSDrawLine ( xi0, eta, xi1, eta );
  if ( dx > 0 ) {
    PSSetLineWidth ( 1.5 );
    x = x0;
    while ( x < xmax ) {
      xi = MapX ( x );
      PSDrawLine ( xi, eta-10.0, xi, eta+10.0 );
      x = x + dx;
    }
    x = x0-dx;
    while ( x >= xmin ) {
      xi = MapX ( x );
      PSDrawLine ( xi, eta-10.0, xi, eta+10.0 );
      x = x - dx;
    }
  }
         /* rysuj os pionowa */
  eta0 = MapY ( ymin );
  eta1 = MapY ( ymax );
  xi = MapX ( x0 );
  PSSetLineWidth ( 3.0 );
  PSDrawLine ( xi, eta0, xi, eta1 );
  if ( dy > 0 ) {
    PSSetLineWidth ( 1.5 );
    y = y0;
    while ( y < ymax ) {
      eta = MapY ( y );
      PSDrawLine ( xi-10.0, eta, xi+10.0, eta );
      y = y + dy;
    }
    y = y0 - dy;
    while ( y >= ymin ) {
      eta = MapY ( y );
      PSDrawLine ( xi-10.0, eta, xi+10.0, eta );
      y = y - dy;
    }
  }
} /*DrawAxes*/

void PlotGraph ( FLOAT xmin, FLOAT xmax,
                 FLOAT (*f)(void *usrptr,FLOAT x), void *usrptr )
{
  point p[NN+1];
  FLOAT dx, x;
  int   i;

  dx = (xmax-xmin)/NN;
  x = xmin;
  p[0].x = MapX ( x );
  p[0].y = MapY ( f ( usrptr, x ) );
  for ( i = 1; i <= NN; i++ ) {
    x = x + dx;
    p[i].x = MapX ( x );
    p[i].y = MapY ( f ( usrptr, x ) );
  }
  PSSetLineWidth ( 4.0 );
  PSDrawPolyline ( NN+1, p, 0 );
} /*PlotGraph*/

int main ( void )
{
  char      fn[] = "wykres.ps";
  int       i, n;
  wielomian w, h;
  spline3   s;
  BSpline   bs;
  FLOAT     v[MAXBSKNOTS-1];

  PSOpenFile ( fn, 600, 8, 7, 250, 150 );
  InitMapping ( -10.0, 11.0, -0.05, 0.25,
                100.0, 2100.0, 100.0, 1200.0 );
  DrawAxes ( -10.0, 11.0, 0.5*PI, 0.0, -0.05, 0.25, 0.25, 0.0 );
  PSSetRGB ( 1.0, 0.0, 0.0 );
  PlotGraph ( -10.0, 10.0, ff, NULL );
  n = w.n = 6;
  for ( i = 0; i <= n; i++ ) {
    w.x[i] = -10.0 + (FLOAT)i/(FLOAT)n*20.0;
/*    w.x[i] = 10.0*cos ( (2.0*(FLOAT)i+1.0)/(2.0*(FLOAT)n+2.0)*PI ); */
    w.a[i] = ff ( NULL, w.x[i] );
  }
        /* wielomian interpolacyjny Lagrange'a - algorytm Aitkena */
/*
  PSSetRGB ( 0.0, 1.0, 0.0 );
  PlotGraph ( -10.0, 10.0, Aitken, &w );
*/
        /* wielomian interpolacyjny Lagrange'a - algorytm roznic dzielonych */
        /* i schemat Hornera */
/*
  Lagrange ( &w, &h);
  PSSetRGB ( 0.0, 0.0, 1.0 );
  PlotGraph ( -10.0, 10.0, npoly, &h );
*/
        /* kubiczna funkcja sklejana, reprezentacja Hermite'a */
/*
  n = s.N = 6;
  for ( i = 0; i <= n; i++ ) {
    s.u[i] = -10.0 + (FLOAT)i/(FLOAT)n*20.0;
    s.a[i] = ff ( NULL, s.u[i] );
  }
  if ( ConstructSpline ( &s ) ) {
    PSSetRGB ( 0.0, 0.5, 1.0 );
    PlotGraph ( -10.0, 10.0, EvalSpline, &s );
  }
*/
        /* kwadratowa funkcja sklejana, reprezentacja B-sklejana */
  n = 7;  /* liczba wezlow interpolacyjnych */
           /* tworzymy wezly */
  for ( i = 0; i < n; i++ ) {
    v[i] = -10.0 + (FLOAT)i/(FLOAT)(n-1)*20.0;
/*    v[i] = -10.0*cos ( (2.0*(FLOAT)i+1.0)/(2.0*(FLOAT)n)*PI ); */
  }
           /* konstruujemy funkcje sklejana */
  if ( ConstructInterpBSpline2 ( n, v, ff, NULL, &bs ) ) {
             /* o.k. - mozna rysowac */
    PSSetRGB ( 0.5, 0.0, 0.75 );
    PlotGraph ( -10.0, 10.0, EvalBSpline, &bs );
  }

  PSCloseFile ();
  printf ( "%s\n", fn );
} /*main*/
