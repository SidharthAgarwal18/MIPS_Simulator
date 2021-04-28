#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;

#include "basic.hpp"

void type_a(string name,int memory[][256],int instruction,int pos,int core,int total_cores)	//for add,slt,mult,addi
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
			
	if(pos+2>=n) throw invalid_argument("Unexpected input1");			//substring for register should pe possible
	
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected input2");			//no valid register
	
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<21);
	pos +=3;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			// for ws after 1st register
	if(pos==n) throw invalid_argument("Unexpected input3");				//input length smaller.
	
	if(int(name[pos]) == 44) 							//1 comma after register1
	{	
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected input4");
	
	if(pos+2>=n) throw invalid_argument("Unexpected input5");			//substring for register should pe possible
	reg = name.substr(pos,3);							//register2
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r==-1) throw invalid_argument("Unexpected inputb");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<16);
	pos += 3;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//{ws+}{,}{ws+}
	if(pos==n) throw invalid_argument("Unexpected input6");				//input length smaller.
	
	if(int(name[pos]) == 44) 
	{
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected input7");
	
	if(pos+2>=n) throw invalid_argument("Unexpected input8");			//substring for register should pe possible
	reg = name.substr(pos,3);							//register3			
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r==-1) throw invalid_argument("Unexpected inputb");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<11);
	pos += 3;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos<n) throw invalid_argument("Unexpected inputatype_a");				//no trash character allowed after instruction
}

void type_b(string name,int memory[][256],int instruction,int pos,int core,int total_cores)				//for addi
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputb");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputatype_b");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<21);
	pos +=3;
	
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos==n) throw invalid_argument("Unexpected inputc");				//input length smaller.
	if(int(name[pos]) == 44)
	{
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected inputd");
	
	if(pos+2>=n) throw invalid_argument("Unexpected inpute");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r==-1) throw invalid_argument("Unexpected inputb");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<16);
	pos += 3;

	//cerr << name[pos];
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos==n) throw invalid_argument("Unexpected inputf");				//input length smaller.
	if(int(name[pos]) == 44)
	{	
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected inputgxys");
	
	if(pos==n) throw invalid_argument("Unexpected inputh");				//input length smaller.

	if(name[pos]==45)														//sign bit is stored in 16th bit
	{memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] +(1<<15); pos++;}
	

	int address = StringtoNumber(name.substr(pos,n-pos),instruction);
	if(address > (1<<15)) throw invalid_argument("Unexpected input "+to_string(instruction)+"address greater than 2^15-1");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + address; //address(or no. for addition)is stored in the 15 least sig. bits		
}

void type_c(string name,int memory[][256],int instruction,int pos,unordered_map<string,int> label,int core,int total_cores)
{
	int n = name.length();
	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
	
	string name_label;
	int first = pos;
	while(pos<n && (int(name[pos])!=9 && int(name[pos])!=32)) pos++;
	name_label = name.substr(first,pos-first);

	if(label.find(name_label)==label.end()) throw invalid_argument("label not found in the program");

	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + label[name_label];
}	

void type_d(string name,int memory[][256],int instruction,int pos,int core,int total_cores)	//for lw and sw
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputj");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputatype_d");			//no valid register
	
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<21);
	pos +=3;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			// for ws after 1st register
	if(pos==n) throw invalid_argument("Unexpected inputk");				//input length smaller.
	if(int(name[pos]) == 44) 							//1 comma after register1
	{	
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected inputl");

	if(name[pos]==45)														//sign bit is stored in 16th bit
	{memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] +(1<<15); pos++;}
	
	int number_end = pos;
	while(number_end<n && name[number_end]-'0'>=0 && name[number_end] -'9'<=0) number_end++;
	if(number_end==pos || number_end==n) throw invalid_argument("Unexpected inputm");
	
	int offset = StringtoNumber(name.substr(pos,number_end-pos),instruction);
	if(offset > (1<<15)) throw invalid_argument("Unexpected input "+to_string(instruction)+" |offset| greater than 2^15");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + offset;
	
	pos = number_end;
	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos==n || name[pos]!=40) throw invalid_argument("Unexpected inputn");	//left parenthesis shoudl be present
	pos = pos +1;
	
	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputo");			//substring for register should pe possible
	
	reg = name.substr(pos,3);							//register2
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r==-1) throw invalid_argument("Unexpected inputp");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<16);
	pos += 3;
	
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos==n || name[pos]!=41) throw invalid_argument("Unexpected inputq");
	pos = pos+1;
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos<n) throw invalid_argument("Unexpected type_d");				//no trash character allowed after instruction
		
}

void type_e(string name,int memory[][256],int instruction,int pos,unordered_map<string,int> label,int core,int total_cores)
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputb");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputatype_e");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<21);
	pos +=3;
	
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos==n) throw invalid_argument("Unexpected inputc");				//input length smaller.
	if(int(name[pos]) == 44)
	{
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected inputd");
	
	if(pos+2>=n) throw invalid_argument("Unexpected inpute");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r==-1) throw invalid_argument("Unexpected inputb");
	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + (r<<16);
	pos += 3;

	//cerr << name[pos];
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos==n) throw invalid_argument("Unexpected inputf");				//input length smaller.
	if(int(name[pos]) == 44)
	{	
		pos++;
		while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	}
	else throw invalid_argument("Unexpected inputgxys");
	
	if(pos==n) throw invalid_argument("Unexpected inputh");				//input length smaller.

	string name_label = name.substr(pos,n-pos);
	if(label.find(name_label)==label.end()) throw invalid_argument("label not found in the program");

	memory[instruction/256 + (1024/total_cores)*core][instruction%256] = memory[instruction/256 + (1024/total_cores)*core][instruction%256] + label[name_label];
}


pair<int,int> map_instruction(string name, int instruction,int memory[][256],int core,int total_cores)
{
	int i = 0;
	int n = name.length();
	
	while(i<n&&(int(name[i])==9||int(name[i])==32)) i++;
	vector<string> types = {"add ","sub ","mul ","slt ","bne ","beq ","j ","lw ","sw ","addi "};
	
	for (int l=0;l<10;l++)
	{
		string cur = types[l];
		int m = cur.size();
		bool pos = true;
		
		for(int j=0;pos&&j<m;j++)
		{
			if((i+j)>=n) pos = false;
			if(pos && name[j+i] != cur[j]) pos = false;
		}
		
		if(pos){
		memory[instruction/256 + (1024/total_cores)*core][instruction%256] = (l+1)<<26;
		return make_pair(l+1,i+m);}
	}
	
	if (i<n) throw invalid_argument("Unexpected input "+to_string(instruction));
	return make_pair(0,0);												//for empty line....
}


void read_and_save_instruction(string instruction_string,int memory[][256],int instruction,unordered_map<string,int> label,int core,int total_cores)
{
	pair<int,int> temp = map_instruction(instruction_string,instruction,memory,core,total_cores);
	int type = temp.first;
	int pos = temp.second;
	if(type==1 || type==2 || type==3 || type ==4) type_a(instruction_string,memory,instruction,pos,core,total_cores);
	else if(type==10) type_b(instruction_string,memory,instruction,pos,core,total_cores);
	else if (type==7)type_c(instruction_string,memory,instruction,pos,label,core,total_cores);
	else if(type==8 || type==9)type_d(instruction_string,memory,instruction,pos,core,total_cores);
	else if (type==5 || type==6)type_e(instruction_string,memory,instruction,pos,label,core,total_cores);
}