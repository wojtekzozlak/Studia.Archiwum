#include<iostream>
using namespace std;

int main(){
  int n;
  cin >> n;
  int *c1 = new int[n];
  int *c2 = new int[n];
  int *c3 = new int[n];
  for(int i=0; i<n; i++){
    cin >> c1[i] >> c2[i] >> c3[i];
  }

  int wyn = n;
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      if(c1[i] < c1[j] && c2[i] < c2[j] && c3[i] < c3[j]){
        wyn--;
        break;
      }
    }
  }

  cout << wyn;

  return 0;
}
