#include<iostream>
#include<vector>
#include<algorithm>

int main(){
  std::vector<int> tab;

  while(true){
    int a;
    std::cin >> a;
    if(std::cin.fail()) break;
    tab.push_back(a);
  }

  std::sort(tab.begin(), tab.end());
  for(int i = 0; i < tab.size(); i++)
    std::cout << tab[i] << " ";
  std::cout << std::endl;

  return 0;
}
