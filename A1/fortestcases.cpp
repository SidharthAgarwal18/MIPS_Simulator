/*
	* Handle - i_m_mohit 
	* Author - Mohit Thakur
*/

#include<bits/stdc++.h>
using namespace std;

#define pb push_back
#define F first
#define S second
#define mp make_pair

long double ans = 0;
int n;
vector<pair<int,int>> b;
int mod = 46;
int scal = 9;
void printinfile()
{
	cout.precision(10);
	cout<<n;
	cout<<"\n";
	for(int i=0;i<n;i++)
	{
		cout<<b[i].F<<"\n"<<b[i].S<<"\n";
	}
	cout<<"\n\n\n\n\n"<<fixed<<ans;
}

int main()
{
	ios_base::sync_with_stdio(0);
	cin.tie(0); cout.tie(0);
	#ifndef ONLINE_JUDGE 
    freopen("in.in", "r", stdin); 
    freopen("case9.a", "w", stdout);  
	#endif
	n=6;
	b.resize(0);
	srand(time(0));
	int x = rand()%mod - scal;
	int y = rand()%mod - scal;
	b.pb(mp(x,y));
	for(int i=0;i<n-1;i++)
	{
		int dx = rand()%mod;
		int yn = rand()%mod - scal;
		long double t = ((dx)*(yn+y));
		ans += t/2;
		x = x+dx;y = yn;
		b.pb(mp(x,y));
	}
	cerr<<ans;
	printinfile();

	cerr<<"time taken : "<<(float)clock()/CLOCKS_PER_SEC<<" secs"<<"\n";
	return 0;
}