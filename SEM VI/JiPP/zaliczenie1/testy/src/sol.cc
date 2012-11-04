#include<iostream>
#include<sstream>
#include<set>

#define N 100000
#define BUFFER 10000


std::set<int> tab[N];

void dfs(int node, int *flags){
  if(flags[node] != 0) return;
  std::cout << node << " ";
  flags[node] = 1;
  for(std::set<int>::iterator it = tab[node].begin(); it != tab[node].end(); it++)
    dfs(*it, flags);
}

int main(){
  int flags[N];

  while(true){
    char bufor[BUFFER];
    int node, a;

    std::cin.getline(bufor, BUFFER);
    if(std::cin.fail()) break;

    std::stringstream line(bufor);
    line >> node;
    while(true){
      line >> a;
      if(line.fail()) break;
      tab[node].insert(a);
    }
  }

  for(int i = 0; i < N; i++){
    flags[i] = 0;
  }
  for(std::set<int>::iterator it = tab[1].begin(); it != tab[1].end(); it++)
    dfs(*it, flags);

  return 0;
}
