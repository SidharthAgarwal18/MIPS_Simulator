#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;


int strCmp(string s1,string s2)
{
	if(s1==s2)
		return 0;
	return 1;
}

int R_index(string name)
{
	if(strCmp(name,"$ze")==0) return 0;
	else if(strCmp(name,"$at")==0) return 1;
	else if(strCmp(name,"$v0")==0) return 2;
	else if(strCmp(name,"$v1")==0) return 3;
	else if(strCmp(name,"$a0")==0) return 4;
	else if(strCmp(name,"$a1")==0) return 5;
	else if(strCmp(name,"$a2")==0) return 6;
	else if(strCmp(name,"$a3")==0) return 7;
	else if(strCmp(name,"$t0")==0) return 8;
	else if(strCmp(name,"$t1")==0) return 9;
	else if(strCmp(name,"$t2")==0) return 10;
	else if(strCmp(name,"$t3")==0) return 11;
	else if(strCmp(name,"$t4")==0) return 12;
	else if(strCmp(name,"$t5")==0) return 13;
	else if(strCmp(name,"$t6")==0) return 14;
	else if(strCmp(name,"$t7")==0) return 15;
	else if(strCmp(name,"$s0")==0) return 16;
	else if(strCmp(name,"$s1")==0) return 17;
	else if(strCmp(name,"$s2")==0) return 18;
	else if(strCmp(name,"$s3")==0) return 19;
	else if(strCmp(name,"$s4")==0) return 20;
	else if(strCmp(name,"$s5")==0) return 21;
	else if(strCmp(name,"$s6")==0) return 22;
	else if(strCmp(name,"$s7")==0) return 23;
	else if(strCmp(name,"$t8")==0) return 24;
	else if(strCmp(name,"$t9")==0) return 25;
	else if(strCmp(name,"$k0")==0) return 26;
	else if(strCmp(name,"$k1")==0) return 27;
	else if(strCmp(name,"$gp")==0) return 28;
	else if(strCmp(name,"$sp")==0) return 29;
	else if(strCmp(name,"$fp")==0) return 30;
	else if(strCmp(name,"$ra")==0) return 31;
	return -1;
}
