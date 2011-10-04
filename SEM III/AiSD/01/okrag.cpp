#include<iostream>
#include<stdio.h>
using namespace std;

main(){
  int a, b, cur; // początek i koniec odcinka; obecny odcinek
  int n; // ilosc punktow
  int l = 0; // dlugosc okregu; a wlasciwie jego polowa
  int best = 0;

  scanf("%d", &n);

  int odl[n+1]; // tablica odleglosci ; {odl[i]} to odleglosc miedzy i-tym, a i+1szym (mod n)

  for(int i=1; i<=n; i++){ // wczytanie
    scanf("%d", &odl[i]);
    l += odl[i];
  }
  l /= 2;

  a = 1;
  b = 2;
  cur = 0;
  while(a <= n){
    while(cur + odl[b] <= l){ // przesuwamy wskaźnik ile się da
      cur += odl[b];
      b += 1;
      if(b == (n+1)) b = 1;
    }
    // sprawdzamy nowego kandydata
    if(best < cur) best = cur;
    a++;
    cur -= odl[a];
  }

  printf("%d\n", best);

  return 0;
}
