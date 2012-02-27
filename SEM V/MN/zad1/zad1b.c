#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "f2c.h"
#include "clapack.h"
#include "newton.h"

#define IND(n,i,j) ((n)*(j)+(i))

#define MAX_TRIES 10

/* Dzieli wielomian [w1] przez wielomian [w2] i zwraca resztę w tablicy [wynik]. */
void mod_wielomian(int n1, FLOAT w1[], int n2, FLOAT w2[], FLOAT wynik[]){
  int i, j;
  FLOAT a;
  FLOAT *kw1 = (FLOAT *) malloc(sizeof(FLOAT) * n1);
  memcpy(kw1, w1, sizeof(FLOAT) * n1);

  for(i = 0; i < n1 - n2 + 1; i++){
    a = kw1[i];
    for(j = 0; j < n2; j++)
      kw1[i + j] -= a * w2[j];
  }


  for(i = 0; i < n2 - 1; i++)
    wynik[i] = kw1[n1 - n2 + 1 + i];

  return ;
}

static long int wielomian_wymiar = 0;
static FLOAT *wielomian;

/* wymiar, śmieć, argument, wartość, wartość pochodnej */
void wielomian_funkcja(long int n, void *usrptr, FLOAT *x, FLOAT *f, FLOAT *Df){
  FLOAT mod[2];
  FLOAT div[3];
  FLOAT h;

  div[0] = 1;
  div[1] = x[0];
  div[2] = x[1];

  if(n != 2){
    printf("func err\n");
    exit(1);
  }

  mod_wielomian(wielomian_wymiar, wielomian, 3, div, mod);
  f[0] = mod[0];
  f[1] = mod[1];

  h = x[0] / 1.0e+12;
  div[1] = x[0] + h;
  div[2] = x[1];
  mod_wielomian(wielomian_wymiar, wielomian, 3, div, mod);
  Df[IND(2,0,0)] = (mod[0] - f[0]) / h; /* pochodna f_1 po x */
  Df[IND(2,1,0)] = (mod[1] - f[1]) / h; /* pochodna f_2 po x */

  h = x[1] / 1.0e+12;
  div[1] = x[0];
  div[2] = x[1] + h;
  mod_wielomian(wielomian_wymiar, wielomian, 3, div, mod);
  Df[IND(2,0,1)] = (mod[0] - f[0]) / h; /* pochodna f_1 po y */
  Df[IND(2,1,1)] = (mod[1] - f[1]) / h; /* pochodna f_2 po y */

  return ;
}

void drukuj_wielomian(int n, FLOAT *w){
  char started = 0, sign;
  int i;
  for(i = 0; i < n; i++){
    if(w[i] == 0)
      continue;
    sign = w[i] > 0 ? '+' : '-';
    if(started != 0 || sign != '+')
      printf("%c ", sign);
    if(w[i] != 0)
      started = 1;

    printf("%f", abs(w[i]));
    if(i != n - 1){
      printf("*x");
      if(i != n - 2)
        printf("^%d", n - i - 1);
      printf(" ");
    }
  }
  return ;
}

/* Rozwiązuje równanie kwadratowe w dziedzinie liczb rzeczywistych. */
void x2_solve(FLOAT *x){
  FLOAT delta = x[1] * x[1] - 4 * x[0] * x[2];
  if(delta == 0){
    printf("%f\n", -x[1] / (2 * x[0]));
  } else if(delta > 0){
    printf("%f\n", (-x[1] + sqrt(delta)) / (2 * x[0]));
    printf("%f\n", (-x[1] - sqrt(delta)) / (2 * x[0]));
  }
  return ;
}

/* Dzieli wielomian przez trójmian. Zakłada, że jest podzielny. */
void div_wielomian(int n1, FLOAT *x, int n2, FLOAT *div){
  int i, j;
  FLOAT a;
  for(i = 0; i < n1 - n2; i++){
     a = x[i] / div[0];
     for(j = 1; j < n2; j++)
       x[i + j] -= a * div[j];
     x[i] = a;
  }
  return ;
}

void wczytaj_wielomian(void){
  int i;
  printf("Podaj stopień wielomianu:\n");
  scanf("%ld", &wielomian_wymiar);
  wielomian_wymiar++;
  wielomian = (FLOAT *) malloc(sizeof(FLOAT) * wielomian_wymiar);
  printf("Podaj współczynniki wielomianu, począwszy od największego stopnia:\n");
  for(i = 0; i < wielomian_wymiar; i++)
    scanf("%lf", &wielomian[i]);
  return ;
}


void losuj_wektor(int n, FLOAT v[]){
  int i;
  for(i = 0; i < n; i++){
    v[i] = rand() / (((double) RAND_MAX) + 1);
    v[i] = rand() % 2 ? -v[i] : v[i];
    v[i] *= 50;
  }
  return ;
}

int faktoryzuj_wielomian(void){
  FLOAT div[3];
  FLOAT trojmiany[1000][3];
  int i, t = 0;
  char ret;

  while(wielomian_wymiar > 3){
    div[0] = 1.0;

    printf("wielomian: \n");
    drukuj_wielomian(wielomian_wymiar, wielomian);
    printf("\n\n");

    for(i = 0; i < MAX_TRIES; i++){
      losuj_wektor(2, div + 1);
      ret = SysNewton(2, wielomian_funkcja, NULL, 1.0e-8, 1.0e-7, 20, div + 1);
      if(ret)
        break;
    }
    if(i == MAX_TRIES){
      printf("Nie udało się rozłożyć wielomianu :(\n");
      return 0;
    }

    div_wielomian(wielomian_wymiar, wielomian, 3, div);
    trojmiany[t][0] = div[0];
    trojmiany[t][1] = div[1];
    trojmiany[t][2] = div[2];

    t++;
    wielomian_wymiar -= 2;
  }
  trojmiany[t][0] = wielomian_wymiar == 3 ? wielomian[0] : 0;
  trojmiany[t][1] = wielomian_wymiar == 3 ? wielomian[1] : wielomian[0];
  trojmiany[t][2] = wielomian_wymiar == 3 ? wielomian[2] : wielomian[1];
  t++;
  

  printf("Rozkład wielomianu na trójmiany:\n");
  for(i = 0; i < t; i++){
    drukuj_wielomian(3, &trojmiany[i][0]);
    printf("\n");
  }
  printf("\n\n");

  printf("Pierwiastki rzeczywiste wielomianu:\n");
  for(i = 0; i < t; i++){
    if(i != t - 1 || wielomian_wymiar == 3)
      x2_solve(&trojmiany[i][0]);
    else
      printf("%f\n", -trojmiany[i][2]);
  }

  return 1;
}

int main(){
  wczytaj_wielomian();
  printf("\n\n");
  faktoryzuj_wielomian();
  srand(time(0));


  return 0;
}
