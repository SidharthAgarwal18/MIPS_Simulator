/*
	* Handle - i_m_mohit 
	* Author - Mohit Thakur
*/

#include <bits/stdc++.h>
using namespace std;

int main()
{
	string instruction_string;
    ifstream MyReadFile("testcase1.txt");
    freopen("out.out", "w", stdout);  
	while(getline (MyReadFile, instruction_string))
	{
		cout << " " << instruction_string;
	}
	MyReadFile.close();
	cout<<"\n\n\n\n\n";
	MyReadFile.open("testcase1.txt");
	while(getline (MyReadFile, instruction_string))
	{
		cout << " " << instruction_string;
	}
	cerr<<"time taken : "<<(float)clock()/CLOCKS_PER_SEC<<" secs"<<"\n";
	return 0;
}
