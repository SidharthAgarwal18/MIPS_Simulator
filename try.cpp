/*
	* Handle - i_m_mohit 
	* Author - Mohit Thakur
*/

#include <bits/stdc++.h>
using namespace std;

string return_label(string name)
{
	int pos = 0;
	int n = name.length();
	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction

	if(pos>=n || name[pos]==':') throw invalid_argument("Unexpected label");

	string label;
	int first = pos;
	while(pos<n && name[pos]!=':') pos++;
	label = name.substr(first,pos-first);
	pos++;
	while(pos<n &&(int(name[pos])==9||int(name[pos])==32)) pos++;

	if(pos<n) throw invalid_argument("Unexpected label");
	return label;
}

int main()
{
	string instruction_string = "    ht : is:   ";
	cout << return_label(instruction_string)<<"\n";
   
	cerr<<"time taken : "<<(float)clock()/CLOCKS_PER_SEC<<" secs"<<"\n";
	return 0;
}
