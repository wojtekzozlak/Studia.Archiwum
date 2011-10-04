#include<stdio.h>

int main()
{
  const int mod = 1000000007;
  int buff;
  int i, j, ind;
  int n, l, r;
  int*tab;

  scanf("%d %d %d\n", &n, &l, &r);  
  char * naw = new char [2 * n + 1];
  tab = new int[n + 2];

  for(i=1; i<=2*n; i++) naw[i] = 0; // dowolnosc

  for(i=0; i<l; i++)
  {
    scanf("%d", &buff);
    naw[buff] = 1; // tylko lewy
  }
  scanf("\n");

  for(i=0; i<r; i++)
  {
    scanf("%d", &buff);
    if(naw[buff] != 1) naw[buff] = -1; // tylko prawy
    else
    {
      printf("0");
      return 0;
    }
  }
  scanf("\n");

  for(i=0; i<n+2; i++)
      tab[i] = 0;

  tab[0] = 1; // wartosc startowa

  for(j=0; j<=n; j++)
  {
    if(j != 0) tab[j] = 0;
    for(i=j; i<=n; i++)
    {
      ind = j + i;
      buff = 0;
      if(naw[ind] >= 0) // lewy
        buff += tab[i];
      if(naw[ind] <= 0) // prawy
        buff += tab[i+1];
      tab[i+1] = buff % mod;
    }
  }


  printf("%d", tab[n+1]);

  return 0;
}
