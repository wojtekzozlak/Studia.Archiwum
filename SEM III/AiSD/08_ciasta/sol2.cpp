#include<iostream>
#include<stdio.h>
#include<algorithm>
using namespace std;

int d = 1;
int *tab;

inline int min(int a, int b){
  return (a < b ? a : b);
}

inline int max(int a, int b){
  return (a > b ? a : b);
}

struct Tree {
  Tree *l, *p;
  int x, y;
  int count;

  Tree(int _x, int _y){
    l = p = NULL;
    x = _x;
    y = _y;
    count = 0;
  }

  inline void check_and_create(){
    if(l == NULL){
      l = new Tree(x, (x + y)/2);
      p = new Tree((x + y)/2 + 1, y);
    }
  }

  void set(int a, int offset){
    if(x <= a && a <= y)
      count += offset;
    if(a < x || y < a || (x == y)) return ;
    check_and_create();
    l->set(a, offset);
    p->set(a, offset);
    return ; 
  }

  int find(int a, int b){
    if(x == a && b == y){
      return count;
    }
    int sum = 0;
    if(a < x || y < b) return sum;
    if(x == y) return sum;
    if(l != NULL){
      sum += l->find(a, min(b, (x + y)/2));
      sum += p->find(max(a, (x + y)/2 + 1), b);
    }
    return sum; 
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
  Tree tree;

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

  int wyn = n;
  for(int i=0; i<n; i++){
    Triple c = ciasta[i];
    XY.set(c.c3, 1);
  }

  for(int i=0; i<n; i++){
    Triple c = ciasta[i];
    if(tree.find(c.c2 + 1, n, c.c3 + 1, n) > 0) wyn--;
    tree.set(c.c2, c.c3, -1);
  }

  printf("%d", wyn);
  return 0;
}
