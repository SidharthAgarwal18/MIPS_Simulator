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
	int saved_address;
	Node* next;
	Node* prev;
	Node()
	{
		data = 0;
		saved_address = 0;
	}
	Node(int x)
	{
		data = x;
		saved_address = 0;
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
	int N,M;    					//Number of files and cycles allowed
	if(argc<5 || argc>5)			//default values...
	{
		row_delay = 10;
		col_delay = 2;		
		N = 3;
		M = 200;
	}
	else
	{
		N = atoi(argv[1]);		// MAX value allowed is 16..
		M = atoi(argv[2]);
		row_delay = atoi(argv[3]);
		col_delay = atoi(argv[4]);
	}
	string output_file_name = "out.txt";
	int memory[1024][256];
	int R[16][32] = {0};
	int busy[16][32] = {0};
	int buffer_row = -1;
	int buffer[256];
	string hash[32];

	bool blocked[16] = {false};
	bool empty_dram = true;
	int row_updates = 0;		//Implement this later
	int num_sw = 0;				//This too..

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

	

	int instruction = -1;
	int end_of_instruction = 0;
	int prev_dram_ins = -1;
	string instruction_string;
	int instruction2 = 0;

	freopen("out.txt","w",stdout);

	for(int I=0;I<N;I++)				//for file reading
	{
		unordered_map<string,int> label;
		string input_file_name = "t" + to_string(I+1) + ".txt";
		ifstream cinstrm(input_file_name);				//reads all the labels
		while(getline(cinstrm,instruction_string))
		{
			if(instruction_string.find(":") != string::npos)
			{
				string name_label = return_label(instruction_string);
				if(label.find(name_label)!=label.end()) throw invalid_argument("Same label repeated");

				label[name_label] = instruction+1;
				if(name_label=="main") main_instruction[I] = instruction+1;
				if(name_label=="exit") exit_instruction[I] = instruction+1;
			}
			else if(is_emptyline(instruction_string)==false)	instruction++;
		}
		cinstrm.close();
		cinstrm.open(input_file_name);
		
		while(getline(cinstrm,instruction_string))			//second reading of the file for normal instructions.
		{
			if(instruction_string.find(":") == string::npos && (is_emptyline(instruction_string)==false))	//no :
			{read_and_save_instruction(instruction_string,memory,instruction2,label);instruction2++;}
		}
		cinstrm.close();
		end_of_instruction = instruction2;
	}

	int R_used[end_of_instruction] = {0};
	int cycle[16] = {1};
	int req_cycle[16] = {0};

	int cur_instruction[16];
	for(int i=0;i<N;i++)
	{
		cycle[i] = 1;
		cur_instruction[i] = main_instruction[i];
	}


	while(true)
	{
		for(int I=0;I<N;I++)
		{
		if(cur_instruction[I] >= main_instruction[I] && cur_instruction[I] < exit_instruction[I])
		{
		if(req_cycle[I] == cycle[I] && buffer_row!=-1)
		{
			empty_dram = true;
			if(prev_dram_ins!=-1)
			{free(prev_dram_ins,R_used,busy[I]);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;

				int add = temp->saved_address;
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				req_cycle[I] = cycle[I] + col_delay;
				
				cout<<"line number "<<ins<<" : cycle "<<cycle[I]<<" - "<<req_cycle[I]-1;
				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[(R_used[ins])]<<endl;

				empty_dram = false;
				blocked[I] = false;
				cycle[I]++;
				continue;
			}
			else if(/*blocked && */head->next->data != -1)
			{
				Node* temp = head -> next;
				int ins = temp-> data;
				prev_dram_ins = ins;

				int add = temp->saved_address;
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
								
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle[I] = cycle[I] + 2*row_delay+ col_delay+1;
					row_updates++;
				}
				else req_cycle[I] = cycle[I] + row_delay+ col_delay+1;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);

				cout<<"line number "<<ins<<" : cycle "<<cycle[I]<< ": DRAM request issued "<<endl;
				cout<<"line number "<<ins<<" : cycle "<<cycle[I]+1<<" - "<<req_cycle[I]-1;
				if((memory_instruction>>26 & ((1<<5)-1))== 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[I][(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				empty_dram = false;
				blocked[I] = false;
				cycle[I]++;
				continue;
			}
		}
		int memory_instruction = memory[cur_instruction[I]/256][cur_instruction[I]%256];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		int prev_instruction = cur_instruction[I];

		if(type==1 || type==2 || type==3 || type==4) cur_instruction[I] = decode_a(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle[I],hash);
		else if(type==10) cur_instruction[I] = decode_b(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle[I],hash);
		else if(type==7) cur_instruction[I] = decode_c(memory_instruction,end_of_instruction,cur_instruction[I],cycle[I]);
		else if(type==5 || type==6) cur_instruction[I] = decode_e(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle[I],hash);
		else if(type == 8 || type ==9)
		{
			if(buffer_row == -1)
			{
				int add = address_of_instruction(memory_instruction,R[I],end_of_instruction);
				int row = add/1024;
				copy_row(memory,buffer,row);
				buffer_row = row;

				req_cycle[I] = cycle[I] + row_delay +col_delay+1;
				decode_d(memory_instruction,R[I],cur_instruction[I],type,end_of_instruction,busy[I],R_used,buffer);
				
				if(type==9) {num_sw++;row_updates++;}

				prev_dram_ins = cur_instruction[I];
				cout<<"line number "<<cur_instruction[I]<<" : cycle "<<cycle[I]<< ": DRAM request issued "<<endl;		//Line number will be wrongly printed.. Sort thiss
				cout<<"line number "<<cur_instruction[I]<<" : cycle "<<cycle[I]+1<<" - "<<req_cycle[I]-1;

				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[cur_instruction[I]])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[I][(R_used[cur_instruction[I]])]<<endl;

				empty_dram = false;
				cur_instruction[I]++;
			}
			else
			{
				//cout << busy[10];
				if(cur_instruction[I] != decode_d(memory_instruction,R[I],cur_instruction[I],type,end_of_instruction,busy[I],R_used,buffer))
				{
					//cout<<instruction<<endl;
					int add = address_of_instruction(memory_instruction,R[I],end_of_instruction);
					Node* temp = new Node(cur_instruction[I]);
					temp->saved_address = add;

					if(type==9) row_updates++;

					temp -> prev = tail -> prev;
					temp -> next = tail;
					tail->prev->next = temp;
					tail->prev = temp;

					same_row[add/1024].push(temp);
					cur_instruction[I]++;
					if(empty_dram) cycle[I]--;		//if dram is empty it will be executed in the same cycle.
				}
			}
		}
		else throw invalid_argument("Unexpected memory instruction");
		if(cur_instruction[I] == prev_instruction) {blocked[I] = true;cycle[I] = req_cycle[I]-1;}
		else blocked[I] = false;

		cycle[I] ++;
		if(cur_instruction[I]==exit_instruction[I]) break;
		req_cycle[I] = max(req_cycle[I],cycle[I]);
		//cout<<req_cycle<<" "<<cycle<<endl;
	}
	}
	}

	//remaining dram instructions.

	
}