#include<iostream>
#include<stdio.h>
#include<vector>
#include<algorithm>
using namespace std;

typedef vector< pair<int, int> > vp;
typedef pair<int, int> ip;

int d;
int **tab;

int _n, _i, _j, _c, _i0, _t;

void set(int a, int b, int v, int t[])
{
  for(int i=a; i<=b; i++)
    t[i] += v;
}

long long find(int v, int t)
{
  return tab[t][v];
}

void popraw(long long x, int T)
{
  long long buf = (x + T)*(x + T)*(x + T);

  _i =  1 + (buf % _n);
  _j = _i + (buf % (_n - _i + 1));
  _c = buf % 10;
  _i0 = 1 + ((x + 1 + T)*(x + 1 + T)*(x + 1 + T) % _n);
  _t = (x + 1234567) % (T + 1);
/*  int __i =  1 + (buf % _n);
  int __j = _i + (buf % (_n - _i + 1));
  int __c = buf % 10;
  int __i0 = 1 + ((x + 1 + T)*(x + 1 + T)*(x + 1 + T) % _n);
  int __t = (x + 1234567) % (T + 1);
  _i = __i;
  _j = __j;
  _c = __c;
  _i0 = __i0;
  _t = __t;*/
}

int main()
{
  scanf("%d", &_n);
  d = 1;
  while(d < _n) d *= 2;
  tab = new int*[_n+1];
  for(int i = 0; i <= _n; i++){
    tab[i] = new int[_n+1];
    for(int j = 0; j <= _n+1; j++) tab[i][j] = 0;
  }

  popraw(0, 0);
  long long x;
  int T;
  for(T = 1; T <= _n; T++)
  {
    for(int j = 1; j <= _n; j++)
      tab[T][j] = tab[T-1][j];
    set(_i, _j, _c, tab[T]);
    cout << T << ": ZWIEKSZ " << _i << " " << _j << " " << _c << " PYTANIE " << _i0 << " " << _t << endl;
    x = find(_i0, _t);
    cout << "x: " << x << endl;
    for(int j = 1; j <= _n; j++)
      cout << tab[T][j] << " ";
    cout << endl;
    popraw(x, T);
  }
  for(int i = 1; i <= _n; i++){
    cout << tab[_n][i] << endl;
  }

  return 0;
}
