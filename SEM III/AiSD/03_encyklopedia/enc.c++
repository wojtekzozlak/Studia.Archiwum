#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int d;
int *tab;

void setMax(int pos, int value)
{
  int a, b, i; // obecny przedzial i wskaznik tablicy
  i = 1;
  a = 1;
  b = d;
  while(a != b)
  {
    if(tab[i] < value) tab[i] = value;
    if(pos <= (a+b)/2)
    {
      i = i << 1;
      b = (a+b)/2;
    }
    else
    {
      i = (i << 1) + 1;
      a = (a+b)/2 + 1;
    }
  }
  if(tab[i] < value) tab[i] = value;
}

inline
int max(int a, int b)
{
  return (a > b ? a : b);
}

inline
int min(int a, int b)
{
  return (a < b ? a : b);
}

int findMaxP(int a, int b, int i, int x, int y)
{
  if(a > b) return 0;
  if(b < x || a > y) return 0;
  if(a == x && b == y) return tab[i];
  else {
    int r1 = findMaxP(a, min((x+y)>>1, b), i<<1, x, (x+y)>>1);
    int r2 = findMaxP(max(((x+y)>>1)+1, a), b, (i<<1)+1, ((x+y)>>1) + 1, y);
    return max(r1, r2);
  } 
}

int findMax(int a, int b)
{
  int i = 1;
  int x = 1;
  int y = d;

  return findMaxP(a, b, i, x, y);
}

int main(){

  int n, i;
  scanf("%d\n", &n);

  char *str = new char[4000000];
  int *dane = new int[n];
  char *pch;
  gets(str);
  pch = strtok(str, " ");

  i = 0;
  while(i < n)
  {
    dane[i] = atoi(pch);
    pch = strtok(NULL, " ");
    i++;
  }
  delete []str;

  d = 1;
  while(d < n) d = d<<1;

  tab = new int[2*d];
  for(i=0; i<2*d; i++) tab[i] = 0;

  int max;
  setMax(dane[0], 1);
  for(i=1; i<n; i++){
    max = findMax(1, dane[i]-1) + 1;
    setMax(dane[i], max);
  }

  printf("%d", n-findMax(1, n));
  return 0; 
}

