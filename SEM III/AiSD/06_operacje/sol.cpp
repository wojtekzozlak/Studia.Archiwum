#include<iostream>
#include<stdio.h>
#include<vector>
#include<algorithm>
using namespace std;

typedef vector< pair<int, int> > vp;
typedef pair<int, int> ip;

int d;
vp *tab;

int _n, _i, _j, _c, _i0, _t;

void set(int a, int b, int l, int r, int ind, int v, int t){
  if(a <= l && r <= b){
    tab[ind].push_back( ip(t, tab[ind].back().second + v) );
    return ;
  }
  if(b < l || a > r) return;
  if(l != r){
    set(a, b, l, (l+r)/2, 2*ind, v, t);
    set(a, b, ((l+r)/2) + 1, r, 2*ind + 1, v, t);
  }
}

long long find(int v, int t, int l, int r, int i)
{
  long long ret = 0;
  vp::iterator z;
  while(l != r){
    z = lower_bound(tab[i].begin(), tab[i].end(), ip(t, -1));
    if(z == tab[i].end()) ret += tab[i].back().second;
    else if(z->first == t) ret += z->second;
    else {
      z--;
      ret += z->second;
    }
    if(v <= (l+r)/2){
      i *= 2;
      r = (l+r)/2;
    } else {
      i = i*2 + 1;
      l = ((l+r)/2) + 1;
    }
  }
  z = lower_bound(tab[i].begin(), tab[i].end(), ip(t, -1));
  if(z == tab[i].end()) ret += tab[i].back().second;
  else if(z->first == t) ret += z->second;
  else {
    z--;
    ret += z->second;
  }

  return ret;
}

long long find_last(int v, int l, int r, int i)
{
  long long ret = 0;
  while(l != r){
    ret += tab[i].back().second;
    if(v <= (l+r)/2){
      i *= 2;
      r = (l+r)/2;
    } else {
      i = i*2 + 1;
      l = ((l+r)/2) + 1;
    }
  }
  ret += tab[i].back().second;

  return ret;
}

void popraw(long long x, int T)
{
  long long buf = (x + T)*(x + T)*(x + T);

  _i =  1 + (buf % _n);
  _j = _i + (buf % (_n - _i + 1));
  _c = buf % 10;
  _i0 = 1 + ((x + 1 + T)*(x + 1 + T)*(x + 1 + T) % _n);
  _t = (x + 1234567) % (T + 1);
}

int main()
{
  scanf("%d", &_n);
  d = 1;
  while(d < _n) d *= 2;
  tab = new vp[2*d];
  for(int i = 0; i < 2*d; i++) tab[i].push_back( ip(0, 0) );

  popraw(0, 0);
  long long x;
  for(int T = 1; T <= _n; T++)
  {
    set(_i, _j, 1, d, 1, _c, T);
    x = find(_i0, _t, 1, d, 1);
    popraw(x, T);
  }
  for(int i = 1; i <= _n; i++){
    cout << find_last(i, 1, d, 1) << endl;
  }

  return 0;
}
