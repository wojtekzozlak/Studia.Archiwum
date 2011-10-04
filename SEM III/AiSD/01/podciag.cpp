#include<stdio.h>

int main(){
  int n, m;
  scanf("%d %d", &n, &m);

  // będziemy rozpinać nasz przedział przez dwa prefixy (odejmujemy krótszy od dłuższego)
  // stąd przedział rozpięty na dwóch prefixach o takiej samej sumie modulo, dadzą resztę 0

  long long tab[n];
  for(int i=0; i<n; i++){
    scanf("%lld", &tab[i]);
    tab[i] %= m;
  }

  long long pre[n];
  pre[0] = tab[0];
  for(int i=1; i<n; i++){
    pre[i] = (pre[i-1] + tab[i]) % m;
  }

  long long kub[m];
  for(int i=0; i<m; i++) kub[i] = 0;
  for(int i=0; i<n; i++) kub[pre[i]]++;

  long long wynik = 1 + kub[0];
  for(int i=0; i<m; i++) wynik += (kub[i]*(kub[i]-1)/2);

  printf("%lld", wynik);
  

  return 0;
}
