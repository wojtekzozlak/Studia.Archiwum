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
  int max;
  int x, y;

  Tree(int _x, int _y){
    l = p = NULL;
    x = _x;
    y = _y;
    max = 0;
  }

  inline void check_and_create(){
    if(l == NULL){
      l = new Tree(x, (x + y)/2);
      p = new Tree((x + y)/2 + 1, y);
    }
  }

  void set(IntPair a){ // first is a c2, second is c3
    if(x <= a.first && a.first <= y && max < a.second){
      max = a.second;
    }
    if(a.first < x || y < a.first || (x == y)) return ;
    check_and_create();
    l->set(a);
    p->set(a);
    return ; 
  }

  int find(int a, int b){
    int ret = 0;
    if(x == a && b == y)
      return max;
    if(a > b || a < x || y < b) return 0; // no value
    if(x == y)
      return max; // no lookup
    if(l != NULL){
      ret = fmax(l->find(a, fmin(b, (x + y)/2)), ret);
      ret = fmax(p->find(fmax(a, (x + y)/2 + 1), b), ret);
    }
    return ret; 
  }
};

struct Triple {
  int c1, c2, c3;
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
    if(maxTree.find(c.c2 + 1, d) > c.c3)
       wyn--;
    maxTree.set(make_pair(c.c2, c.c3));
  }

  printf("%d", wyn);
  return 0;
}
