#include<iostream>
#include<utility>
#include<algorithm>
#include<stdio.h>
using namespace std;

#define WALL_MIN 999999999
#define WALL_MAX -999999999

#define MNIEJSZE 0
#define WIEKSZE 1
#define MEDIANA 2

pair<int, int> *lookup;

bool fmax(int a, int b){
  return a > b;
}

bool fmin(int a, int b){
  return a < b;
}

struct Kopiec {
  int max; // maksymalny rozmiar kopca
  pair<int, int> *tab; // elty kopca <klucz, lookup>
  int n, wall, id; // pierwszy wolny index, strażnik, id
  long long count; // suma
  bool (*cmp)(int, int); // komparator
  
  Kopiec(int max, bool (*cmp)(int, int), int wall, int id){
    int d = 1;
    while(d < max) d *= 2;
    this->max = d;
    this->tab = new pair<int, int>[d + 1];
    for(int i=1; i<=d; i++)
      this->tab[i].first = wall;
    this->cmp = cmp;
    this->wall = wall;
    this->id = id;
    this->count = 0;
    n = 1;
  }

  inline void refresh(int a){
    if(tab[a].first != wall)
      lookup[tab[a].second] = make_pair(id, a);
  }

  pair<int, int> buff;
  void swap(int a, int b){
    buff = tab[b];
    tab[b] = tab[a];
    tab[a] = buff;
    refresh(a);
    refresh(b);
  }

  void incKey(int key, int pos){
    tab[pos].first = key;
    pair<int, int> buff;
    while(pos != 1 && !cmp(tab[pos/2].first, key)){ // operator
      swap(pos, pos/2);
      pos /= 2;
    }
  }

  void insert(pair<int, int> elt){
    count += elt.first;
    tab[n] = elt;
    refresh(n);
    incKey(elt.first, n);
    n++;
  }

  void adjust(int pos){
    while(pos < max/2){
      if(!cmp(tab[pos].first, tab[2*pos].first) ) // operator
        if(!cmp(tab[2*pos].first, tab[2*pos + 1].first)){
          swap(pos, 2*pos + 1);
          pos = 2*pos + 1;
        } else {
          swap(pos, 2*pos);
          pos = 2*pos;
        }
      else if(!cmp(tab[pos].first, tab[2*pos + 1].first)){
        swap(pos, 2*pos + 1);
        pos = 2*pos + 1;
      } else break;
    }
  }

  pair<int, int> get(){
    if(n == 1) return make_pair(-1, -1);
    pair<int, int> ret = tab[1];
    remove(1);
    return ret;
  }

  void remove(int pos){
    n--;
    count -= tab[pos].first;
    tab[pos] = tab[n];
    refresh(pos);
    tab[n].first = wall;
    incKey(tab[pos].first, pos);
    adjust(pos);
  }

  long long size(){
    return n-1;
  }
};

Kopiec *mniejsze;
Kopiec *wieksze;
int n, k;
pair<int, int> mediana = make_pair(-1, -1);

void balance(){
  if(wieksze->size() > 0 && mediana.first == -1) mediana = wieksze->get();
  while(mniejsze->size() > ((k-1)/2)){
    if(mediana.first != -1) wieksze->insert(mediana);
    mediana = mniejsze->get();
  }
  while(mniejsze->size() < ((k-1)/2)){
    if(mediana.first != -1) mniejsze->insert(mediana);
    mediana = wieksze->get();
  }
  if(mediana.first != -1){
    lookup[mediana.second] = make_pair(MEDIANA, 0);
  }
}

int main(){
  scanf("%d %d", &n, &k);
  mniejsze = new Kopiec(k + 4, fmax, WALL_MAX, MNIEJSZE);
  wieksze = new Kopiec(k + 4, fmin, WALL_MIN, WIEKSZE);
  lookup = new pair<int, int>[n+2];
  int tab[n]; // elty
  for(int i=0; i<n; i++)
    scanf("%d", &tab[i]);


  int najInd, najMed;
  long long najKoszt = 999999999999999999ll;
  long long bufKoszt;
  for(int i=0; i<k-1; i++){
    wieksze->insert(make_pair(tab[i], i));
  }
  balance();
  for(int i=k-1; i<n; i++){
    if(tab[i] > mediana.first) wieksze->insert(make_pair(tab[i], i));
    else mniejsze->insert(make_pair(tab[i], i));
    balance();
    bufKoszt = mniejsze->size()*mediana.first - mniejsze->count +
               wieksze->count - wieksze->size()*mediana.first;
    if(bufKoszt < najKoszt){
      najKoszt = bufKoszt;
      najInd = i-k+1;
      najMed = mediana.first;
    }
    if(lookup[i - k + 1].first == MNIEJSZE) mniejsze->remove(lookup[i - k + 1].second);
    else if(lookup[i - k + 1].first == WIEKSZE) wieksze->remove(lookup[i - k + 1].second);
    else {
      mediana = wieksze->get();
      lookup[mediana.second] = make_pair(MEDIANA, 0);
    }
  }
  printf("%lld\n", najKoszt);
  for(int i=0; i<najInd; i++) printf("%d\n", tab[i]);
  for(int i=0; i<k; i++) printf("%d\n", najMed);
  for(int i=najInd+k; i<n; i++) printf("%d\n", tab[i]);
  return 0;
}
