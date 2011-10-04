#include<stdio.h>
#include<utility>
#include<vector>
#include<set>
using namespace std;

#define koszt first.first
#define numer first.second
#define stopien second

int main(){
  int n, m, k;
  scanf("%d %d %d", &n, &m, &k);
  pair< pair<long long, int>, int > tab[n+1]; // pary koszt, stopień
  vector<int> zalezne[n+1]; // projekty zależne
  for(int i=1; i<=n; i++){
    scanf("%d", &tab[i].koszt);
    tab[i].stopien = 0;
    tab[i].numer = i;
  }
  int a, b;
  for(int i=0; i<m; i++){
    scanf("%d %d", &a, &b);
    zalezne[b].push_back(a);
    tab[a].stopien++;
  }

  multiset< pair<long long, int> > liscie;
  for(int i=1; i<=n; i++){
    if(tab[i].stopien == 0) liscie.insert(tab[i].first);
  }

  long long max = 0;
  pair<long long, int> buff;
  for(int i=0; i<k; i++){
    buff = *liscie.begin();
    if(buff.first > max) max = buff.first;
    int count = zalezne[buff.second].size();
    for(int j=0; j<count; j++){
      tab[zalezne[buff.second][j]].stopien--;
      if(tab[zalezne[buff.second][j]].stopien == 0)
        liscie.insert(tab[zalezne[buff.second][j]].first);
    }
    liscie.erase(buff);
  }
  printf("%lld", max);

  return 0;
}
