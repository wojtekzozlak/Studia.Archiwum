#include<iostream>
#include<stdio.h>
using namespace std;

int main()
{
  long long n, m;
  long long x;
  scanf("%lld", &m);
  
  for(long long i=1; i<=m; i++)
  {
    scanf("%lld", &n);
    if(n%2 == 1) x = (6*(n*n*n) + 15*(n*n) + 8*n - 5)/24;
    else x = (n*n*n + 5*n*n + 2*n)/8;
    printf("%lld\n", x);
  }


  return 0;
}