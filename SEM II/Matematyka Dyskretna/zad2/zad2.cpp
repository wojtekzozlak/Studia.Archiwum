#include<iostream>
#include<stdio.h>
#include<math.h>
using namespace std;

long long troj4(long long n)
{
  return (  n*(n+1)*(2*n + 3) - 2*(n - n/2)  )/8;
}


double troj2(long long k)
{
  double n = (double)k;
  return ( n*(n+1)*(2*n + 3) - 2*ceil(n/2)  )/8;
}

double troj1(long long x)
{
  double n = (double)x;
  double wynik;
  wynik = 0;

  wynik += n*(n+1)*(n+2)/6;

  wynik += n*(n+1)*(2*n+1)/24;

  wynik -= ceil(n/2)/4;
  //for(long long k=1; k<=(long long)n; k++){
//      wynik += (double)k*k

    //wynik -= ((double)(k%2))/4;
//    for(long long l=0; l <= ((k/2) -1); l++) wynik += k - 1 - 2*l;
  //}

  return wynik;
}

int main()
{
  long long n, m;

  scanf("%lld", &n);
  
  for(long i=1; i<=n; i++)
  {
    scanf("%lld", &m);
    printf("%lld %lf %lf\n", troj4(m), troj2(m), troj1(m));
  }


  return 0;
}