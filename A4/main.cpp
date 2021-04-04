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
	string input_file_name = "input.txt";
	string output_file_name = "out.txt";
	int memory[1024][256];
	int R[32] = {0};
	int busy[32] = {0};
	int buffer_row = -1;
	int buffer[256];
	string hash[32];

	bool blocked = false;
	bool empty_dram = true;
	int row_updates = 0;
	int num_sw = 0;

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
	int prev_dram_ins = -1;
	string instruction_string;

	freopen("out.txt","w",stdout);
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
		if(req_cycle == cycle && buffer_row!=-1)
		{
			empty_dram = true;
			if(prev_dram_ins!=-1)
			{free(prev_dram_ins,R_used,busy);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				req_cycle = cycle + col_delay;
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);

				cout<<"line number "<<ins<<" : cycle "<<cycle<<" - "<<req_cycle-1;
				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;

				empty_dram = false;
				blocked = false;
				cycle++;
				continue;
			}
			else if(/*blocked && */head->next->data != -1)
			{
				Node* temp = head -> next;
				int ins = temp-> data;
				prev_dram_ins = ins;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);
				
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay;
					row_updates++;
				}
				else req_cycle = cycle + row_delay+ col_delay;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);

				cout<<"line number "<<ins<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"line number "<<ins<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;
				if((memory_instruction>>26 & ((1<<5)-1))== 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				empty_dram = false;
				blocked = false;
				cycle++;
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

				req_cycle = cycle + row_delay +col_delay;
				decode_d(memory_instruction,R,instruction,type,end_of_instruction,busy,R_used,buffer);
				
				if(type==9) {num_sw++;row_updates++;}

				prev_dram_ins = instruction;
				cout<<"line number "<<instruction<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"line number "<<instruction<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;

				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[instruction])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[instruction])]<<endl;

				empty_dram = false;
				instruction++;
			}
			else
			{
				//cout << busy[10];
				if(instruction != decode_d(memory_instruction,R,instruction,type,end_of_instruction,busy,R_used,buffer))
				{
					//cout<<instruction<<endl;
					int add = address_of_instruction(memory_instruction,R,end_of_instruction);
					Node* temp = new Node(instruction);

					if(type==9) row_updates++;

					temp -> prev = tail -> prev;
					temp -> next = tail;
					tail->prev->next = temp;
					tail->prev = temp;

					same_row[add/1024].push(temp);
					instruction++;
					if(empty_dram) cycle--;		//if dram is empty it will be executed in the same cycle.
				}
			}
		}
		else throw invalid_argument("Unexpected memory instruction");
		if(instruction == prev_instruction) {blocked = true;cycle = req_cycle-1;}
		else blocked = false;

		cycle ++;
		req_cycle = max(req_cycle,cycle);
		//cout<<req_cycle<<" "<<cycle<<endl;
	}




	//remaining dram instructions.

	while(head->next->data != -1)
	{
		if(req_cycle == cycle)
		{
			empty_dram = true;
			if(prev_dram_ins!=-1)
			{free(prev_dram_ins,R_used,busy);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				req_cycle = cycle + col_delay;
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);

				cout<<"line number "<<ins<<" : cycle "<<cycle<<" - "<<req_cycle-1;

				if((memory_instruction>>26 & ((1<<5)-1))== 8)
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
				prev_dram_ins = ins;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
	
				int memory_instruction = memory[ins/256][ins%256];
				int add = address_of_instruction(memory_instruction,R,end_of_instruction);
				
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay;
					row_updates++;
				}
				else req_cycle = cycle + row_delay+ col_delay;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);
				
				cout<<"line number "<<ins<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"line number "<<ins<<" : cycle "<<cycle<<" - "<<req_cycle-1;
				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				cycle++;
				continue;
			}
		}
		
		cycle = req_cycle;
	}

	if(prev_dram_ins!=-1)
	{free(prev_dram_ins,R_used,busy);prev_dram_ins = -1;}

	if(num_sw!=0)
	{
		write_row(memory,buffer,buffer_row);
		req_cycle = cycle + 2*row_delay+ col_delay;
		row_updates++;	
	}

	cout<<"\nTotal number of cycles : "<<max(cycle,req_cycle)-1<<endl<<"Total number of row buffer updates : "<<row_updates<<endl;
	return 0;
}