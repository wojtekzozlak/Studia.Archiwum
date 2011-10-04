#include <stdlib.h>
#include <list>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;

int n, m;
int *tab;
int *counters;
int *sizes;
int k;
vector<int> *edges;
list< pair<int, int*> > sets; // counter of sets

void popraw(int i, int n)
{
  int j;
  while(tab[i] != -1){ // TODO : skracanie sciezek
    j = tab[i];
    tab[i] = n;
    i = j;
  }
  return ;
}

int find(int i)
{
  int bak = i;
  while(tab[i] != -1){ // TODO : skracanie sciezek
    i = tab[i];
  }
  popraw(bak, i);
  return i;
}

int set_union(int a, int b) // drugi zostaje nowym reprezentantem
{
  a = find(a);
  b = find(b);
  if(a != b)
  {
    tab[a] = b;
  }
  return b;
}


int main()
{
//  cout << "start" << endl;

  int nn;
  scanf("%d %d", &n, &m);
  nn = n;
  tab = new int[n];
  counters = new int[n];
  sizes = new int[n];
  edges = new vector<int>[n];
  for(int i=0; i<n; i++)
  {
    tab[i] = -1;
    sizes[i] = 1;
    sets.push_back(make_pair(i, &counters[i]));
  }

  for(int i=0; i<m; i++)
  {
    int a, b;
    scanf("%d %d", &a, &b);
    a -= 1;
    b -= 1;
    edges[a].push_back(b);
    edges[b].push_back(a);
  }


  list< pair<int, int*> >::iterator it2;  
  for(int i=0; i<nn; i++)
  {
    for(it2 = sets.begin(); it2 != sets.end(); it2++) *it2->second = 0;

    for(int j=0; j<edges[i].size(); j++)
      counters[find(edges[i][j])]++; // zwiekszam licznik u reprezentanta
    
    for(it2 = sets.begin(); it2 != sets.end();)
    {
      if(*it2->second < sizes[it2->first] && it2->first != find(i)) // jesli licznik polaczen jest rozny od rozmiaru zbioru, to trzeba je polaczyc
      {
        set_union(it2->first, i); // dolaczamy do tego z glownej petli
        sizes[find(i)] += sizes[it2->first];
        sizes[it2->first] = 0;
        it2 = sets.erase(it2); // usuwamy podlaczony z listy zbiorow
        n--;
      } else it2++;
    }
  }

  sort(sizes, sizes + nn);
  printf("%d\n", n);
  for(int i=0; i<nn; i++)
    if(sizes[i] != 0) printf("%d ", sizes[i]);

  return 0;
}
