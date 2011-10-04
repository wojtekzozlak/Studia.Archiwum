#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<time.h>
using namespace std;

int main(){
	srand(time(0));
	int n;
	cin >> n;

	int t1[n];
	int t2[n];
	int t3[n];
	for(int i=0; i<n; i++)
		t1[i] = t2[i] = t3[i] = (i + 1);

	random_shuffle(t1, t1+n);
	random_shuffle(t2, t2+n);
	random_shuffle(t3, t3+n);

	cout << n << endl;
	for(int i=0; i<n; i++)
		cout << t1[i] << " " << t2[i] << " " << t3[i] << endl;
	return 0;
}
