#include<iostream>
#include<vector>
#include<time.h>
#include<stdlib.h>
#include<map>

#define ILOSC 10
#define V 10
#define E 100
#define OFFSET_V 5
#define OFFSET_E 5
#define ETYKIETY 500
using namespace std;

int main()
{
  srand(time(NULL));

  cout << ILOSC << endl << endl << endl;
  
  for(int i=0; i<ILOSC; i++)
  {
	// czy jest cykl?
    bool cykl = ( (rand()%2) == 0 );


	// tworzenie krawędzi
    vector< vector<int> > tab;
    int size_v = V + ( rand() % OFFSET_V);
    tab.resize(size_v);
    int size_e = E + ( rand() % OFFSET_E);
    for(int j=0; j<size_e; j++)
    {
      int x = rand() % size_v;
      int y = rand() % size_v;
      if( x < y )
      {
        int x_size = tab[x].size();
        bool flag = false;
        for(int k=0; k<x_size; k++) if(tab[x][k] == y) flag = true;
        if(!flag) tab[x].push_back(y);
      }
    }
    
    // dodanie cyklu
    if(cykl)
    {
	  int dlugosc = rand()%(V/10) + 2;
	  int start = rand() % size_v;
	  int x = start;
	  int y;
	  for(int j=0; j<dlugosc; j++)
      {
	    do
	    {
          y = rand() % size_v;
	    }
	    while(x == y);
        tab[x].push_back(y);
		x = y;
	  }
	  // dopełnienie cyklu
	  tab[x].push_back(start);
    }

	// przeetykietowanie
    map<int, int> edges;
    map<int, int> etykiety;
    
    for(int j=0; j<size_v; j++)
    {
      map<int, int>::iterator it;
      int etq;
      do
      {
        etq = ( rand() % ETYKIETY );
      } while( etykiety.find(etq) != etykiety.end() );
      edges[j] = etq;
      etykiety[etq] = j;
    }

    // output
    int size_real = 0;
    for(int j=0; j<size_v; j++) if(tab[j].size() != 0) size_real++;
    cout << size_real << endl;
    for(int j=0; j<size_v; j++)
    {
      int size_j = tab[j].size();
      if(size_j!=0)
      {
        cout << edges.find(j)->second << " " << size_j << endl;
        for(int k=0; k<size_j; k++) cout << edges.find(tab[j][k])->second << " ";
        cout << endl;
      }
    }
    cout << cykl << endl;
    cout << endl << endl;
  }
  
  return 0;
}
