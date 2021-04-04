#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;

//Variable whose values are not changed frequently..
unordered_map<string,int> label;
int main_instruction = -1;
int exit_instruction = -1;
int row_delay,col_delay;


//Basic utility functions
int StringtoNumber(string name,int instruction)
{
	int number = 0;
	int digit = 0;
	int pos = 0;
	int n = name.length();
	while(pos<n && name[pos]-'0'>=0 && name[pos] -'9'<=0)
	{
		digit = name[pos] - '0';
		number = 10*number + digit; 
		pos++;	
	}
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos<n) throw invalid_argument("Unexpected input " + to_string(instruction) + " : Not all digits");		//no garbage character allowed before, b/w and after number.
	return number;
}


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

void copy_row(int memory[][256],int buffer[],int buffer_row)
{
	for(int i=0;i<256;i++)
	{
		buffer[i] = memory[buffer_row][i];
	}
}

void write_row(int memory[][256],int buffer[],int buffer_row)
{
	for(int i=0;i<256;i++)
	{
		memory[buffer_row][i] = buffer[i];
	}
}


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


bool is_emptyline(string name)					//nowonwards empty line is not counted in instruction.
{
	int n = name.length();
	int pos = 0;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
	if(pos<n) return false;
	return true;
}

int address_of_instruction(int memory_instruction,int R[],int end_of_instruction)
{
	int offset = ((1<<15)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	int sign = (memory_instruction & (1<<15));

	int address;
	if(sign!=0) address = R[r2] - 4*offset;
	else address = R[r2] + 4*offset;

	if(address/4>=((1<<18)) || (address/4<=end_of_instruction) || address<0) 
	{throw invalid_argument("Unexpected inputw " + to_string(memory_instruction)+" because of either access of encoded instruction data in memory or memory size");}
	
	return address;
}
