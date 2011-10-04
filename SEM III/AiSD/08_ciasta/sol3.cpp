#include<iostream>
#include<stdio.h>
#include<algorithm>

#include<vector>
using namespace std;

typedef pair<int, int> IntPair;

int d = 1;
int *tab;

inline int fmin(int a, int b){
  return (a < b ? a : b);
}

inline int fmax(int a, int b){
  return (a > b ? a : b);
}

struct Tree {
  Tree *l, *p;
  vector<IntPair> max;
  int x, y;

  Tree(int _x, int _y){
    l = p = NULL;
    x = _x;
    y = _y;
    max.push_back(make_pair(0, 0));
  }

  inline void check_and_create(){
    if(l == NULL){
      l = new Tree(x, (x + y)/2);
      p = new Tree((x + y)/2 + 1, y);
    }
  }

  void set(IntPair a){ // first is a c2, second is c3
    if(x <= a.first && a.first <= y && max[max.size()-1].second < a.second){
//      printf("w przedziale %d %d ustawiam max na %d %d\n", x, y, a.first, a.second);
      max.push_back(a);
    }
    if(a.first < x || y < a.first || (x == y)) return ;
    check_and_create();
    l->set(a);
    p->set(a);
    return ; 
  }

  void unset(IntPair a){ // first is a c2, second is c3
    if(x <= a.first && a.first <= y && max[max.size()-1] == a){
//      printf("w przedziale %d %d ", x, y);
//      printf("wywalam %d %d\n", max[max.size()-1].first, max[max.size()-1].second);
      max.pop_back();
//      printf("nowy max to %d %d\n", max[max.size()-1].first, max[max.size()-1].second);
    }
    if(a.first < x || y < a.first || (x == y)) return ;
    check_and_create();
    l->set(a);
    p->set(a);
    return ; 
  }


  int find(int a, int b){
    int ret = max[max.size()-1].second;
    if(x <= a && b <= y){
      return ret;
    }
    if(a < x || y < b) return 0; // no value
    if(x == y) return 0; // no lookup
    if(l != NULL){
      ret = fmax(l->find(a, fmin(b, (x + y)/2)), ret);
      ret = fmax(p->find(fmax(a, (x + y)/2 + 1), b), ret);
    }
    return ret; 
  }
};

struct Triple {
  int c1, c2, c3;
  void str(){
    printf("%d %d %d\n", c1, c2, c3);
  }
};

bool cmp1(Triple a, Triple b){
  return (a.c1 < b.c1);
}

bool cmp2(Triple a, Triple b){
  return (a.c2 < b.c2);
}

bool cmp3(Triple a, Triple b){
  return (a.c3 < b.c3);
}

int main(){
  int a, b, c, n;
  scanf("%d", &n);
  while(d < n) d *= 2;
  Tree maxTree(1, d);

  Triple *ciasta = new Triple[n];
  for(int i=0; i<n; i++){
    scanf("%d %d %d", &a, &b, &c);
    ciasta[i].c1 = a;
    ciasta[i].c2 = b;
    ciasta[i].c3 = c;
  }

  sort(ciasta, ciasta + n, cmp3);
  for(int i=0; i<n; i++) ciasta[i].c3 = i + 1;
  sort(ciasta, ciasta + n, cmp2);
  for(int i=0; i<n; i++) ciasta[i].c2 = i + 1;
  sort(ciasta, ciasta + n, cmp1);
  for(int i=0; i<n; i++) ciasta[i].c1 = i + 1;


  sort(ciasta, ciasta + n, cmp3);
  sort(ciasta, ciasta + n, cmp2);
  sort(ciasta, ciasta + n, cmp1);

  int wyn = n;
  for(int i=n-1; i>=0; i--){
    Triple c = ciasta[i];
    maxTree.set(make_pair(c.c2, c.c3));
  }

  for(int i=0; i<n; i++){
    Triple c = ciasta[i];
//    printf("szukam max c3 dla obiektow z c2 z zakresu %d %d\n", c.c2 + 1, d);
    if(maxTree.find(c.c2 + 1, d) > c.c3) wyn--;
//    printf("%d\n", maxTree.find(c.c2 + 1, d));
    maxTree.unset(make_pair(c.c2, c.c3));
  }

  printf("%d", wyn);
  return 0;
}
