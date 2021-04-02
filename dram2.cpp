#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;
#include "basics.hpp"
#include "encode_instructions.hpp"
#include "decode_instructions.hpp"



pair<int,int> map_instruction(string name, int instruction,int memory[][256])
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
		memory[instruction/256][instruction%256] = (l+1)<<26;
		return make_pair(l+1,i+m);}
	}
	
	if (i<n) throw invalid_argument("Unexpected input "+to_string(instruction));
	return make_pair(0,0);												//for empty line....
}


void read_and_save_instruction(string instruction_string,int memory[][256],int instruction,unordered_map<string,int> label)
{
	pair<int,int> temp = map_instruction(instruction_string,instruction,memory);
	int type = temp.first;
	int pos = temp.second;
	if(type==1 || type==2 || type==3 || type ==4) type_a(instruction_string,memory,instruction,pos);
	else if(type==10) type_b(instruction_string,memory,instruction,pos);
	else if (type==7)type_c(instruction_string,memory,instruction,pos,label);
	else if(type==8 || type==9)type_d(instruction_string,memory,instruction,pos);
	else if (type==5 || type==6)type_e(instruction_string,memory,instruction,pos,label);
}


void work_done(int memory_instruction,int busy[],int R_used[])
{
	int s = R_used[memory_instruction];
	if(busy[s]==1) busy[s] = 0;		//unbusy ony when there was single occurence of the instruction
	else if(busy[s]>0)busy[s] = busy[s] - 2;
	return ;
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
	{throw invalid_argument("Unexpected inputw because of either access of encoded instruction data in memory or memory size");}
	
	return address;
}


int main(int argc,char** argv)
{
	int R[32]={0};
	int memory[1024][256]={0};
	int buffer[256] = {0};
	int busy[32] = {0};
	unordered_map<string,int> label;				//maps label to their instruction number.

	bool blocked = false;							//tells wheather there is need to run from dubly linked list or not
	int main_instruction = -1;
	int exit_instruction = 0;						//here program will terminate.
	int row_delay,col_delay,dummy,variable;
	int cycle = 1;
	int buffer_row = -1;
	int row_updates = 0;
	int max_cycle = 0;				//for cycle=max_cycle DRAM will be free for new operation.
	//int num_sw = 0; 				//for checing if there is need for writing back to memory from buffer.

	if(argc<3 || argc>3)			//default values...
	{
		row_delay = 10;
		col_delay = 2;		
	}
	else
	{
		string number(argv[1]);
		row_delay = StringtoNumber(number,-1);
		string number2(argv[2]);
		col_delay = StringtoNumber(number2,-1);
	}

	queue<pair<int,Node* >> same_row[1024]; 	//an array of queues.
	Node* head = new Node(-1);				//head node is unique and doesnot represent any instruction.
	Node* tail = head;
	Node* temp;
	pair<int,Node*> p;

	map<int,string> hash;
	hash[0] = "$zero"; hash[1] = "$at"; hash[2] = "$v0"; hash[3] = "$v1";hash[4] = "$a0";hash[5] = "$a1";hash[6] = "$a2";hash[7] = "$a3";
	hash[8] = "$t0";hash[9] = "$t1";hash[10] = "$t2";hash[11] = "$t3";hash[12] = "$t4";hash[13] = "$t5";hash[14] = "$t6";hash[15] = "$t7";
	hash[16] = "$s0";hash[17] = "$s1";hash[18] = "$s2";hash[19] = "$s3";hash[20] = "$s4";hash[21] = "$s5";hash[22] = "$s6";hash[23] = "$s7";
	hash[24] = "$t8";hash[25] = "$t9";hash[26] = "$k0";hash[27] = "$k1";hash[28] = "$gp";hash[29] = "$sp";hash[30] = "$fp";hash[31] = "$ra";

	int instruction = 0;
	int end_of_instruction = 0; 			//will be equal to exit_instruction+1

	string instruction_string;
	//string instruction_string;
	string name_label;

	ifstream cinstrm("testcase1.txt");			//first reading for reading labels
	//cerr << "HII";
	while(getline(cinstrm,instruction_string))
	{
		//cerr << "ab";
		if(instruction_string.find(":") != string::npos)		// ":" found
		{
			//cerr << "a";
			name_label = return_label(instruction_string);
			//cerr << name_label;
			if(label.find(name_label)!=label.end()) throw invalid_argument("Same label repeated");

			label[name_label] = instruction;
			memory[instruction/256][instruction%256] = (0>>26);	 //for instructions with label.
			if(name_label=="main") main_instruction = instruction;
			if(name_label=="exit") exit_instruction = instruction;
		}
		else if(is_emptyline(instruction_string)==false)	instruction++;
	}
	//cerr << "HI";
	if(label.find("main")==label.end() || label.find("exit")==label.end()) 
	{throw invalid_argument("Either of main or exit label is missing");}

	instruction = 0;
	cinstrm.close();
	cinstrm.open("testcase1.txt");
	freopen("out.txt","w",stdout);
	
	while(getline(cinstrm,instruction_string))			//second reading of the file for normal instructions.
	{
		if(instruction_string.find(":") == string::npos)	//no :
		{read_and_save_instruction(instruction_string,memory,instruction,label);}
		else if(is_emptyline(instruction_string)==true) instruction--;
		instruction++;
	}
	end_of_instruction = instruction;			   
	instruction = main_instruction+1;
	
	int R_used[end_of_instruction+1];	
	while(instruction<end_of_instruction && instruction >=0)
	{
		if(max_cycle==cycle)
		{
			if(buffer_row>=0 && same_row[buffer_row].size()>0) 
			{
				max_cycle = cycle + col_delay;
				p = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				work_done(memory[p.first/256][p.first%256],busy,R_used);
				delete_node(p.second,tail);

				variable = address_of_instruction(memory[p.first/256][p.first%256],R,end_of_instruction);

				cout<<"line number "<<p.first<<" : cycle"<<cycle<<" - "<<max_cycle-1;
				if(memory[p.first/256][p.first%256]>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[instruction])]<<" = ";}
				else cout<<": memory address "<<variable<<"-"<<variable+3<<" = ";
				cout<<R[(R_used[instruction])]<<endl;

				blocked = false;		//if not false then will be made true again in this cycle.
				cycle++;
			}
			else if(head!=tail && blocked==true) //run only when instructions are not moving forward otherwise wait for same row lw/sw
			{
				write_row(memory,buffer,buffer_row);

				temp = head->next;
				delete_node(temp,tail);
				work_done(memory[temp->data/256][temp->data%256],busy,R_used);
				variable = (address_of_instruction(memory[temp->data/256][temp->data%256],R,end_of_instruction));
				buffer_row = variable/1024;
				same_row[buffer_row].pop();

				max_cycle = cycle + row_delay + row_delay + col_delay+1;

				cout<<"cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"line number "<<temp->data<<" : cycles "<<cycle+1<<" - "<<max_cycle-1;
				if(memory[temp->data/256][temp->data%256]>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[instruction])]<<" = ";}
				else cout<<": memory address "<<variable<<"-"<<variable+3<<" = ";
				cout<<R[(R_used[instruction])]<<endl;

				blocked = false;
				cycle++;
			}
		}

		int memory_instruction = memory[instruction/256][instruction%256];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		int prev_instruction = instruction;
		
		// instruction is incremented only if "instruction" has been executed.
		if(type==1 || type==2 || type==3 || type==4) instruction = decode_a(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==10) instruction = decode_b(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==7) instruction = decode_c(memory_instruction,end_of_instruction,instruction,cycle);
		else if ((type ==8 || type == 9))
		{
			if(instruction!=decode_d(memory_instruction,R,instruction,type,end_of_instruction,busy,R_used,buffer))
			{
				temp = new Node(instruction);
				same_row[buffer_row].push(make_pair(instruction,temp));

				tail->next = temp;						//insertion in doubly linked list....
				temp->prev = tail;
				tail = tail->next;
			}
		}
		else if(type==5 || type==6) instruction = decode_e(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if (type==0 || type == 12) instruction = instruction+1;
		
		if(instruction==exit_instruction) break;
		if(prev_instruction==instruction) blocked = true;	//maybe there is a need to change the row buffer....

		cycle++;	
		max_cycle = max(max_cycle,cycle);	
	}
	
	write_row(memory,buffer,buffer_row);
	cout<<endl<<"Total cycles executed : "<<max_cycle-1<<endl<<"Total buffer row updates : "<<row_updates;
	cerr <<"HI";
	return 0;
}
