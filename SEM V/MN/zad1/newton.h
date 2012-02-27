#define __DOUBLE__
#ifndef __DOUBLE__
  #define FLOAT float
  #define GESV sgesv_
#else
  #define FLOAT double
  #define GESV dgesv_
#endif

char SysNewton ( long int n,
                 void (*fd)(long int n, void *usrptr,
                            FLOAT *x, FLOAT *f, FLOAT *Df),
                 void *usrptr, FLOAT eps, FLOAT delta, int maxit,
                 FLOAT *x );

