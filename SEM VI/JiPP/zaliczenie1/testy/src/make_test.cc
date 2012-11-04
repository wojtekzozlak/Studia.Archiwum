#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<set>

int main(){
  srand(time(0));

  int size = 500 + (rand() % 500);
  std::set<int> tab[size + 1];

  for(int i = 0; i < 50 * size; i++){
    tab[rand() % size].insert(rand() % size);
  }

  for(int i = 1; i <= size; i++){
    std::cout << i << " ";
    for(std::set<int>::iterator it = tab[i].begin(); it != tab[i].end(); it++){
      std::cout << *it << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
