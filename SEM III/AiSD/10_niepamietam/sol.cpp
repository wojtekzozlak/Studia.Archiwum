#include <iostream>
#include <stdio.h>
#include <set>
#include <vector>
using namespace std;

#define docelowy first.second
#define bazowy second
#define waga first.first

struct Graf {
  long long *dist;
  vector< pair<int, int> > *edges; // krawedzie wychodzące <waga, do>
  bool *visited;

  Graf(int n){
    dist = new long long[n];
    for(int i=0; i<n; i++)
      dist[i] = 9999999999999999ll;
    edges = new vector< pair<int, int> >[n];
    visited = new bool[n];
    for(int i=0; i<n; i++)
      visited[i] = false;
  }

  void dijkstra(int v){
    set< pair<int, int> > mySet; // <waga, wierzcholek>
    visited[v] = true;
    dist[v] = 0;
    mySet.insert(make_pair(dist[v], v));

    pair<int, int> minV, buff;
    int target, edge;
    while(!mySet.empty()){
      minV = *mySet.begin();
      mySet.erase(minV);

      visited[minV.second] = true;
      for(int i=0; i<edges[minV.second].size(); i++){
        target = edges[minV.second][i].second;
        edge = edges[minV.second][i].first;
        if(dist[target] > dist[minV.second] + edge){
          if(mySet.find(make_pair(dist[target], target)) != mySet.end()) // jesli jest w kopcu 
            mySet.erase(make_pair(dist[target], target)); // to już go nie ma
          dist[target] = dist[minV.second] + edge;
          if(!visited[target])
            mySet.insert(make_pair(dist[target], target));
        }
      }
    }
  }
};

int main(){
  int n, m;
  int a, b, c;
  scanf("%d %d", &n, &m);
  Graf graf1(n);
  Graf graf2(n);
  for(int i=0; i<m; i++){
    scanf("%d %d %d", &a, &b, &c);
    graf1.edges[a].push_back(make_pair(c, b));
    graf2.edges[b].push_back(make_pair(c, a));
  }
  graf1.dijkstra(0);
//  cout << "DIJKSTRA 2" << endl;
  graf2.dijkstra(n-1);

  long long max = 999999999999999999ll;
  long long lmax;
  for(int i=0; i<n; i++){
    lmax = graf1.dist[i] > graf2.dist[i] ? graf1.dist[i] : graf2.dist[i];
    if(lmax < max) max = lmax;
  }
  
/*  for(int i=0; i<n; i++){
    cout << i << ": " << graf1.dist[i] << endl;
  }*/
  cout << max;

  return 0;
}
