#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;

#include "basic.hpp"
#include "saving_instructions.hpp"
#include "decode.hpp"

struct Node
{
	int data;
	Node* next;
	Node* prev;
	Node()
	{
		data = 0;
	}
	Node(int x)
	{
		data = x;
	}
};

void free (int ins,int R_used[],int busy[])
{
	int s = R_used[ins];
	//cout << s << busy[s];
	if(busy[s]==1) busy[s] = 0;
	else if(busy[s]>0) busy[s] = busy[s] - 2;
	//cout <<busy[s];
	return ;
}

int main(int argc,char* argv[])
{
	string input_file_name = "test1.txt";
	string output_file_name = "out1.txt";
	int memory[1024][256];
	int R[32] = {0};
	int busy[32] = {0};
	int buffer_row = -1;
	int buffer[256];
	string hash[32];
	bool blocked = false;
	queue<Node* > same_row[1024];	
	Node* head = new Node(-1);
	Node* tail = new Node(-1);
	head -> next = tail;
	head ->prev = nullptr;
	tail -> prev = head;
	tail -> next = nullptr;
	hash[0] = "$zero"; hash[1] = "$at"; hash[2] = "$v0"; hash[3] = "$v1";hash[4] = "$a0";hash[5] = "$a1";hash[6] = "$a2";hash[7] = "$a3";
	hash[8] = "$t0";hash[9] = "$t1";hash[10] = "$t2";hash[11] = "$t3";hash[12] = "$t4";hash[13] = "$t5";hash[14] = "$t6";hash[15] = "$t7";
	hash[16] = "$s0";hash[17] = "$s1";hash[18] = "$s2";hash[19] = "$s3";hash[20] = "$s4";hash[21] = "$s5";hash[22] = "$s6";hash[23] = "$s7";
	hash[24] = "$t8";hash[25] = "$t9";hash[26] = "$k0";hash[27] = "$k1";hash[28] = "$gp";hash[29] = "$sp";hash[30] = "$fp";hash[31] = "$ra";
	if(argc<3 || argc>3)			//default values...
	{
		row_delay = 10;
		col_delay = 2;		
	}
	else
	{
		row_delay = atoi(argv[1]);
		col_delay = atoi(argv[2]);
	}


	int instruction = -1;
	string instruction_string;

	freopen("out1.txt","w",stdout);
	ifstream cinstrm(input_file_name);				//reads all the labels
	while(getline(cinstrm,instruction_string))
	{
		if(instruction_string.find(":") != string::npos)
		{
			string name_label = return_label(instruction_string);
			if(label.find(name_label)!=label.end()) throw invalid_argument("Same label repeated");

			label[name_label] = instruction+1;
			if(name_label=="main") main_instruction = instruction+1;
			if(name_label=="exit") exit_instruction = instruction+1;
		}
		else if(is_emptyline(instruction_string)==false)	instruction++;
	}
	cinstrm.close();


	cinstrm.open(input_file_name);
	int end_of_instruction = 0;
	instruction = 0;
	while(getline(cinstrm,instruction_string))			//second reading of the file for normal instructions.
	{
		if(instruction_string.find(":") == string::npos && (is_emptyline(instruction_string)==false))	//no :
		{read_and_save_instruction(instruction_string,memory,instruction,label);instruction++;}
	}
	cinstrm.close();
	end_of_instruction = instruction;			   
	instruction = max(main_instruction,0);
	int R_used[end_of_instruction] = {0};


	int cycle = 1;
	int req_cycle = 0;
	while(instruction >= 0 && instruction < end_of_instruction)
	{
		if(req_cycle == cycle)
		{
			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				free(ins,R_used,busy);
				//cout << busy[10];
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
				req_cycle = cycle + col_delay;
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);

				cout<<"line number "<<ins<<" : cycle"<<cycle<<" - "<<req_cycle-1;
				if(memory_instruction>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;
				cycle++;
				blocked = false;
				continue;
			}
			else if(blocked && head->next->data != -1)
			{
				Node* temp = head -> next;
				int ins = temp-> data;
				free(ins,R_used,busy);
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
				req_cycle = cycle +2*row_delay+ col_delay;

				int add = address_of_instruction(memory_instruction,R,end_of_instruction);
				write_row(memory,buffer,buffer_row);
				buffer_row = add/1024;
				copy_row(memory,buffer,buffer_row);

				cout<<"line number "<<ins<<" : cycle"<<cycle<<" - "<<req_cycle-1;
				if(memory_instruction>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;
				cycle++;
				blocked = false;
				continue;
			}
		}
		int memory_instruction = memory[instruction/256][instruction%256];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		int prev_instruction = instruction;
		if(type==1 || type==2 || type==3 || type==4) instruction = decode_a(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==10) instruction = decode_b(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==7) instruction = decode_c(memory_instruction,end_of_instruction,instruction,cycle);
		else if(type==5 || type==6) instruction = decode_e(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type == 8 || type ==9)
		{
			if(buffer_row == -1)
			{
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);
				int row = add/1024;
				copy_row(memory,buffer,row);
				buffer_row = row;
				req_cycle = cycle + row_delay ;
				decode_d(memory_instruction,R,instruction,type,end_of_instruction,busy,R_used,buffer);
				Node* temp = new Node(instruction);
				temp -> prev = tail -> prev;
				temp -> next = tail;
				tail->prev->next = temp;
				tail->prev = temp;
				same_row[add/1024].push(temp);
				cout<<"line number "<<instruction<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				instruction++;
			}
			else
			{
				//cout << busy[10];
				if(instruction != decode_d(memory_instruction,R,instruction,type,end_of_instruction,busy,R_used,buffer))
				{
					//cout << "AAA"<<prev_instruction<<instruction<<"BBB:";
					int add = address_of_instruction(memory_instruction,R,end_of_instruction);
					Node* temp = new Node(instruction);
					temp -> prev = tail -> prev;
					temp -> next = tail;
					tail->prev->next = temp;
					tail->prev = temp;
					same_row[add/1024].push(temp);
					instruction++;
				}
			}
		}
		else throw invalid_argument("Unexpected memory instruction");
		if(instruction == prev_instruction) {blocked = true;cycle = req_cycle-1;}
		cycle ++;
	}

	while(head->next->data != -1)
	{
		if(req_cycle == cycle)
		{
			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				free(ins,R_used,busy);
				//cout << busy[10];
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
				req_cycle = cycle + col_delay;
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);

				cout<<"line number "<<ins<<" : cycle"<<cycle<<" - "<<req_cycle-1;
				if(memory_instruction>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;
				cycle++;
				continue;
			}
			else
			{
				Node* temp = head -> next;
				int ins = temp-> data;
				free(ins,R_used,busy);
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
				req_cycle = cycle +2*row_delay+ col_delay;

				int add = address_of_instruction(memory_instruction,R,end_of_instruction);
				write_row(memory,buffer,buffer_row);
				buffer_row = add/1024;
				copy_row(memory,buffer,buffer_row);

				cout<<"line number "<<ins<<" : cycle"<<cycle<<" - "<<req_cycle-1;
				if(memory_instruction>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;
				cycle++;
				continue;
			}
		}
		
		cycle = req_cycle;
	}

	return 0;
}