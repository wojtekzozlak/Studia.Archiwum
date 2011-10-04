#include<stdlib.h>
#include<stdio.h>

int main(){
  long long int i, j;
  long long int n;
  scanf("%lld\n", &n);
  if(n % 2 == 1){
    printf("-1");
    return 0;
  }

  char *str = new char[n];

  for(int i=0; i<n; i++) scanf("%c", &str[i]);

  // tablica "czy palindrom od A do B"
  bool **czyP = new bool*[n];
  for(i=0; i<n; i++) czyP[i] = new bool[n];
  for(i=0; i<n; i++)
    for(j=0; j<n; j++) czyP[i][j] = false;


  // najpierw sprawdzamy podstawowe pary
  // nie ma dla nas palindromów na indeksach nieparzystych
  // (wtedy nie da się podzielić całego ciągu)
  for(i=0; i<n; i++)
    if(str[i] == str[i+1]) czyP[i][i+1] = true;
  

  // teraz dynamicznie sprawdzamy wszystkie
  for(i=4; i-1<n; i+=2) // skok indeksu
    for(j=0; j+i-1<n; j++){
      if(str[j] == str[j+i-1] && czyP[j+1][j+i-2]) czyP[j][j+i-1] = true;
    }

  // mamy bazę, lecimy z faktycznym zadaniem
  long long int wyniki[n+1];
  long long int min;
  wyniki[0] = 0; // strażnik
  for(i=2; i<=n; i+=2){ // długość naszego ciągu
    min = n;
    for(j=1; j<=i; j+=2){ // miejsce w którym go łamiemy
      if(czyP[j-1][i-1]){
        min = (wyniki[j-1] + 1 < min ? wyniki[j-1] + 1 : min);
      }
    }
    wyniki[i] = min;
  }

  if(min == n) min = -1;
  printf("%lld", min);

  return 0;
}
