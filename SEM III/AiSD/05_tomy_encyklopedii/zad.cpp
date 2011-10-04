#include<stdio.h>
#include<math.h>
#include<iostream>
using namespace std;

long long abs(long long a, long long b)
{
  return (a > b ? a - b : b - a);
}

int *tab;
int *loc;

long long merge(int a1, int a2, int b1, int b2)
{
  long long ret = 0;
  int s = a1;
  int i=0;
  while(a1 <= a2 && b1 <= b2)
  {
    if(tab[a1] < tab[b1]){
      loc[i] = tab[a1];
      a1++;
    } else {
      loc[i] = tab[b1];
      ret += a2 - a1 + 1;
      b1++;
    }
    i++;
  }
  for(; a1<=a2; a1++, i++)
    loc[i] = tab[a1];
  for(; b1<=b2; b1++, i++)
    loc[i] = tab[b1];

  for(i=0; i<(b1-s); i++)
    tab[s+i] = loc[i];

  return ret;
}

long long mergesort(int a, int b)
{
  if(a == b) return 0;
  long long ret = 0;
  ret += mergesort(a, (a+b)/2);
  ret += mergesort(1 + (a+b)/2, b);  
  ret += merge(a, (a+b)/2, 1 + (a+b)/2, b);
  return ret;
}

int main()
{
  int n;
  int counter = 0;
  scanf("%lld\n", &n);
  tab = new int[n+1];
  loc = new int[n+1];

  for(long long i=1; i<=n; i++)
    scanf("%lld", &tab[i]);


  printf("%lld", mergesort(1, n));
}
