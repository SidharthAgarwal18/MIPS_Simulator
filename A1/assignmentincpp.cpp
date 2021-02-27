#include <iostream>
using namespace std;

int main()
{
	double f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12;
	bool flag;
	int n;		//t0 in misp code
	cin>>n;
	int i = 1;	//t1 in misp code
	f1 = 0.0;
	if(n<i) goto finish;
	f2 = 0.0;
	cin >> f0;
	f3 = f0;
	cin >> f0;
	f4 = f0;
whil:
	if(n==i) goto finish;
	i = i+1;
	cin >> f0;
	f5=f0;
	cin>>f0;
	f6 = f0;

	f9 = f6*f4;
	flag = (f0<=f9);
	if(flag) goto ifcase;
	if(!flag) goto elsecase;

continu:
	f7 = f5-f3;
	f3 = f5;
	f4 = f6;

	f5 = f7*f8;
	f8 = 0.5;
	f6 = f5*f8;
	f1 = f1+f6;
	goto whil;

finish:
	f12 = f1;
	cout.precision(10);
	cout<<fixed<<f12;
	return 0;

ifcase:
	f8 = f6+f4;
	f8 = abs(f8);
	goto continu;

elsecase:
	f10 = f4*f4;
	f11 = f6*f6;
	f11 = f11+f10;
	f12 = f6-f4;
	f8 = f11/f12;
	f8 = abs(f8);
	goto continu;
}