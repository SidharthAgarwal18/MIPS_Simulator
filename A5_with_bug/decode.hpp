#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;
#include "basic.hpp"

int decode_a(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],unordered_set<int>& rows_involved_when_blocked,unordered_map<int,int> row_blocking_sw[],unordered_map<int,int> row_blocking_lw[],int priority[])
{
	int r3 = ((1<<5)-1) & (memory_instruction>>11);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(r1==0) throw invalid_argument("An attempt to change the value stored in $zero ");
	if(busy[r3]==1 || busy[r2]==1 || busy[r1]==1 || busy[r1]>=2) 			//if either of them is busy dont move forward
	{
		rows_involved_when_blocked.clear();
		priority[core] = 0;
		blocked[core] = true;
		for(auto x:row_blocking_lw[r3])
		{	
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0 && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}

			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_lw[r2])
		{	
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}

			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_lw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}

			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_sw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		return instruction;
	}
	else
		blocked[core] = false;

	if(op==1) R[r1] = R[r2] + R[r3];
	else if(op==2) R[r1] = R[r2] - R[r3];
	else if(op==3) R[r1] = R[r2]*R[r3];
	else if(op==4 && R[r2]<R[r3]) R[r1] = 1;
	else R[r1] = 0;

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;

	return instruction+1;
}

int decode_b(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],unordered_set<int>& rows_involved_when_blocked,unordered_map<int,int> row_blocking_sw[],unordered_map<int,int> row_blocking_lw[],int priority[])
{
	int address = ((1<<15)-1) & (memory_instruction);		//address is stored in 15 bits now.
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(r1==0) throw invalid_argument("An attempt to change the value stored in $zero ");
	
	if(busy[r2]==1 || busy[r1]==1 || busy[r1]>=2) 							//if either of them is busy dont move forward
	{
		rows_involved_when_blocked.clear();
		priority[core] = 0;
		blocked[core] = true;
		for(auto x:row_blocking_lw[r2])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_lw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_sw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		return instruction;
	}
	else
		blocked[core] = false;

	int sign = (memory_instruction & (1<<15));					//for dealing with negative sign
	if(sign!=0) R[r1] = R[r2] - address;
	else R[r1] = R[r2] + address;

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;
	return instruction+1;
}

int decode_c(int memory_instruction,int end_of_instruction,int instruction,int cycle,int core,int ref_ins,bool blocked[],unordered_set<int>& rows_involved_when_blocked,unordered_map<int,int> row_blocking_sw[],unordered_map<int,int> row_blocking_lw[],int priority[])
{
	int new_instruction = ((1<<26)-1) & (memory_instruction);
	if(new_instruction>end_of_instruction) throw invalid_argument("Unexpected output in jump statement");

	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<new_instruction - ref_ins<<endl;
	return new_instruction;		//correct this.
}

void take_data(int buffer[],int R[],int r1,int location,int remainder)
{
	if(remainder==0) {R[r1] = buffer[location];return ;}

	if(location==255) throw invalid_argument("cannot access memory from two rows at same time");
	switch(remainder)
	{
		case 1: R[r1] = (buffer[location] <<8) + ((buffer[location+1]>>24)&((1<<8) - 1));break;
		case 2: R[r1] = (buffer[location] <<16)+ ((buffer[location+1]>>16)&((1<<16) - 1));break;
		case 3: R[r1] = (buffer[location] <<24)+ ((buffer[location+1]>>8)&((1<<24) - 1));break;
	}
	if(r1==0 && R[r1]!=0) throw invalid_argument("An attempt to change the value stored in $zero ");
	return ;
}

void enter_data(int buffer[],int location,int remainder,int value)
{
	if(remainder==0) {buffer[location] = value; return;}
	
	if(location==255) throw invalid_argument("cannot access memory from two rows at same time");

	int value1 = buffer[location];
	int value2 = buffer[location+1];
	switch(remainder)
	{
		case 1: buffer[location] = ((value1>>24)<<24) + value>>8;
			buffer[location+1] = ((value & ((1<<8)-1))<<24) + (value2 & ((1<<24)-1));
			break;		
		case 2:
			buffer[location] = ((value1>>16)<<16) + value>>16;
			buffer[location+1] = ((value & ((1<<16)-1))<<16) + (value2 & ((1<<16)-1));
			break;
		case 3: 
			buffer[location] = ((value1>>8)<<8) + value>>24;
			buffer[location+1] = ((value & ((1<<24)-1))<<8) + (value2 & ((1<<8)-1));
			break;
	}
	return ;
}

int decode_d(int memory_instruction,int R[],int instruction,int op,int core,int end_of_instruction,int busy[],int R_used[],int buffer[],bool blocked[],unordered_set<int>& rows_involved_when_blocked,unordered_map<int,int> row_blocking_sw[],unordered_map<int,int> row_blocking_lw[],int priority[])
{
	int offset = ((1<<15)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r2]==1 || busy[r1]==1 || (busy[r1]>=2 && op==8)) 
	{
		rows_involved_when_blocked.clear();
		priority[core] = 0;
		blocked[core] = true;
		for(auto x:row_blocking_lw[r2])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_lw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		if(op==8)
		{
			for(auto x:row_blocking_sw[r1])
			{
				if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
				{rows_involved_when_blocked.insert(x.first);}
		
				if(priority[core]==-1) priority[core] = 0;
				priority[core] += ceil(x.second/10);
			}
		}
		return instruction;
	}
	else
	blocked[core] = false;

	//busy[r2] = 1;												//design  decision.
	if(op==8)busy[r1] = 1;						//only r1 is locked for lw and permanently 
	else busy[r1] = busy[r1]+2;							//value of r2 can be accessed but not changed
	R_used[instruction] = r1;
	
	int sign = (memory_instruction & (1<<15));

	int address;
	if(sign!=0) address = R[r2] - 4*offset;
	else address = R[r2] + 4*offset;

	if(address/4>=((1<<18)) || (address/4<=end_of_instruction) || address<0) 
	{throw invalid_argument("Unexpected inputwsdank "+to_string(instruction)+" because of either access of encoded instruction data in memory or address not in memory size");}
		
	if(op==8) take_data(buffer,R,r1,(address/4)%256,address%4)	;		        
	else if(op==9) enter_data(buffer,(address/4)%256,address%4,R[r1]) ;			
	
	return instruction+1;
}

int decode_e(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,string hash[],int core,int ref_ins,bool blocked[],unordered_set<int>& rows_involved_when_blocked,unordered_map<int,int> row_blocking_sw[],unordered_map<int,int> row_blocking_lw[],int priority[])
{
	int next_instruction = ((1<<16)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r1]==1 || busy[r2]==1) 
	{
		rows_involved_when_blocked.clear();
		priority[core] = 0;
		blocked[core] = true;
		for(auto x:row_blocking_lw[r2])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		for(auto x:row_blocking_lw[r1])
		{
			if(rows_involved_when_blocked.find(x.first)!= rows_involved_when_blocked.end() && x.second != 0)
			{rows_involved_when_blocked.insert(x.first);}
		
			if(priority[core]==-1) priority[core] = 0;
			priority[core] += ceil(x.second/10);
		}
		return instruction;
	}
	else
		blocked[core] = false;

	if(op==6 && R[r1]==R[r2]) 
	{
		cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<next_instruction - ref_ins<<endl;
		return next_instruction;
	}
	else if(op==5 && R[r1]!=R[r2]) 
	{
		cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<next_instruction - ref_ins<<endl;
		return next_instruction;
	}
	cout<<"core :"<<core<<" line number "<<instruction - ref_ins<<": cycle "<<cycle<<": "<<"jumped to line number "<<instruction+1 - ref_ins<<endl;
	return instruction+1;
}