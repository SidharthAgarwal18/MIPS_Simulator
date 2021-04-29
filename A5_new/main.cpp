#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;

#include "basic.hpp"
#include "saving_instructions.hpp"
#include "decode.hpp"

void free (int ins,int R_used[],pair<int,Node*> busy[])
{
	int s = R_used[ins];
	if(busy[s].first==1) busy[s].first = 0;
	return ;
}

void print_map(std::unordered_map<int,int> const &m)//Added this functon only for debugging purposes
{
	cerr << "a";
    for (auto &pair: m) {
        cerr << "{" << pair.first << ": " << pair.second << "}\n";
    }
}

Node* memory_manger(Node* head,Node* tail,int N,int buffer_row,bool blocked[])
{
	//return head->next;		//comment this to run after this

	for(auto same_row_ins = head;same_row_ins!=tail;same_row_ins = same_row_ins -> next)
	{
		if(same_row_ins-> saved_address/1024 == buffer_row) return same_row_ins;
	}
	for(int I =0;I<N;I++)
	{
		if(blocked[I] == true)
		{
			auto req_row_ins = head;
			while(req_row_ins != tail)
			{
				if(req_row_ins -> saved_address /1024 == buffer_row) return req_row_ins;
				req_row_ins = req_row_ins->next;
			}
		}
	}
	return head->next;
}

int main(int argc,char* argv[])
{
	int N,M;    					//Number of files and cycles allowed
	int row_delay,col_delay;
	if(argc<5 || argc>5)			//default values...
	{
		row_delay = 10;
		col_delay = 2;		
		N = 3;
		M = 1500;
	}
	else
	{
		N = atoi(argv[1]);		// MAX value allowed is 16..
		M = atoi(argv[2]);
		row_delay = atoi(argv[3]);
		col_delay = atoi(argv[4]);
	}
	string output_file_name = "out.txt";
	int memory[1024][256] = {0};

	int core_in_dram = -1;
	int remaining_cores = N;
	int MAX_WAIT_BUFFER_SIZE = 32;			
	int write_port[N] = {0};				//saves instruction number at which write_port is already busy.
	int R[N][32] = {0};
	pair<int,Node*> busy[N][32];
	int buffer_row = -1;
	int buffer[256];
	string hash[32];

	bool blocked[N] = {false};								//Is core 'I' blocked
	int priority[N];
	for(int i=0;i<N;i++) priority[i] = -1;

	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			R[i][j] =  0;
			busy[i][j].first = 0;
		}
		write_port[i] = 0;
		blocked[i] = false;
	}

	int row_updates = 0;		//Implement this later
	int num_sw = 0;				//This too..
	int wait_buffer_size = 0;

	Node* head = new Node(-1);			//DLL to maintain wait buffer
	Node* tail = new Node(-1);
	head->saved_address = -1025;
	tail->saved_address = -1025;
	head -> next = tail;
	head ->prev = nullptr;
	tail -> prev = head;
	tail -> next = nullptr;

	hash[0] = "$zero"; hash[1] = "$at"; hash[2] = "$v0"; hash[3] = "$v1";hash[4] = "$a0";hash[5] = "$a1";hash[6] = "$a2";hash[7] = "$a3";
	hash[8] = "$t0";hash[9] = "$t1";hash[10] = "$t2";hash[11] = "$t3";hash[12] = "$t4";hash[13] = "$t5";hash[14] = "$t6";hash[15] = "$t7";
	hash[16] = "$s0";hash[17] = "$s1";hash[18] = "$s2";hash[19] = "$s3";hash[20] = "$s4";hash[21] = "$s5";hash[22] = "$s6";hash[23] = "$s7";
	hash[24] = "$t8";hash[25] = "$t9";hash[26] = "$k0";hash[27] = "$k1";hash[28] = "$gp";hash[29] = "$sp";hash[30] = "$fp";hash[31] = "$ra";

	int main_instruction[N] = {-1};
	int exit_instruction[N] = {-1};
	int instruction = -1;
	int prev_dram_ins = -1;
	string instruction_string;
	int instruction2 = 0;

	int pending_instruction =-1;
	int pending_finish = -1;
	int pending_core = -1;

	freopen("out.txt","w",stdout);

	int max_num_ins = INT_MIN;

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
			{read_and_save_instruction(instruction_string,memory,instruction2,label,I,N);
				instruction2++;}
		}
		cinstrm.close();
		max_num_ins = max(max_num_ins,instruction2);
		instruction2 = 0;		//now all instructions start from 0
		instruction = -1;
	}

	int R_used[N][max_num_ins] = {0};
	int cycle = 1;
	int req_cycle = 1;

	int cur_instruction[N];
	for(int i=0;i<N;i++)
	{
		cur_instruction[i] = main_instruction[i];
	}

	while(cycle <= M && remaining_cores>0)
	{
		//cerr << cycle <<" "<< blocked[0] <<endl;
		if(req_cycle == cycle && buffer_row!=-1)
		{
			if(prev_dram_ins!=-1)		//there is actually a dram instuction just finished.
			{free(prev_dram_ins,R_used[core_in_dram],busy[core_in_dram]);prev_dram_ins = -1;}

			if(head->next->data != -1)		//common queue is not empty and buffer_row has no ins.
			{
				Node* temp = memory_manger(head,tail,N,buffer_row,blocked);
				
				int ins = temp-> data;
				int core = temp->core;

				core_in_dram = core;
				prev_dram_ins = ins;

				int add = temp->saved_address;
				int enter_data = temp->data_entered;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				wait_buffer_size--;

				int memory_instruction = memory[ins/256 + core*(1024/N)][ins%256];
						
				if(add/1024 == buffer_row)
				{
					req_cycle = cycle + col_delay;
				}		
				else if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay;
					row_updates++;
					num_sw = 0;
				}
				else {req_cycle = cycle + row_delay+ col_delay;num_sw = 0;}

				buffer_row = add/1024;
				copy_row(memory,buffer,buffer_row);

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					pending_core = core;
					break;
				}				

				//cout<<"core :"<<<<" line number "<<ins <<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"core :"<<core_in_dram<<" line number "<<ins<<": cycle "<<cycle<<" - "<<req_cycle-1;

				int type = (memory_instruction>>26 & ((1<<5)-1));
				if(type == 8)
				{cout<<": "<<hash[(R_used[core_in_dram][ins])]<<" = "<<R[core_in_dram][(R_used[core_in_dram][ins])]<<endl;}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = "<<enter_data<<endl;
				

				write_port[core_in_dram] = req_cycle - 1;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;
			}
		}
		for(int I=0;I<N;I++)		//processing each core
		{
			if(cur_instruction[I] >= main_instruction[I] && cur_instruction[I] < exit_instruction[I])	//program has not yet finished
			{
				//cerr << "B" << I ;
				int memory_instruction = memory[cur_instruction[I]/256 + I*(1024/N)][cur_instruction[I]%256];
				int type = ((1<<5)-1) & (memory_instruction>>26);
				int prev_instruction = cur_instruction[I];

				if((type==1 || type==2 || type==3 || type==4) && write_port[I]!=cycle) cur_instruction[I] = decode_a(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,0,blocked,priority);
				else if(type==10 && write_port[I]!=cycle) cur_instruction[I] = decode_b(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,0,blocked,priority);
				else if(type==7) cur_instruction[I] = decode_c(memory_instruction,exit_instruction[I],cur_instruction[I],cycle,I,0,blocked,priority);
				else if(type==5 || type==6) cur_instruction[I] = decode_e(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,0,blocked,priority);
				else if((type == 8 || type ==9) && wait_buffer_size<MAX_WAIT_BUFFER_SIZE) 
				{
					if(buffer_row == -1)
					{
						int add = address_of_instruction(memory_instruction,R[I],exit_instruction[I],((1024/N)*I*1024));
						int row = add/1024;
						copy_row(memory,buffer,row);		
						buffer_row = row;

						req_cycle = cycle + row_delay +col_delay+1;		//req_cycle is where ins is completed.
						decode_d(memory_instruction,R[I],cur_instruction[I],type,I,exit_instruction[I],busy[I],R_used[I],buffer,blocked,((1024/N)*I*1024),priority);
						
						prev_dram_ins = cur_instruction[I];				//needed for freeing the correct instruction regs. later.
						core_in_dram = I;

						if(req_cycle<=M)
						{	
							if(type==9) {num_sw++;row_updates++;}			//check for need for write_back

							cout<<"core :"<<I<<" line number "<<cur_instruction[I]<<": cycle "<<cycle<< ":  instruction saved in wait buffer"<<endl;		
							cout<<"core :"<<I<<" line number "<<cur_instruction[I]<<": cycle "<<cycle+1<<" - "<<req_cycle-1;

							write_port[I] = req_cycle -1 ;
							if((memory_instruction>>26 & ((1<<5)-1)) == 8)		//different print for lw and sw.
							{cout<<": "<<hash[(R_used[I][cur_instruction[I]])]<<" = ";}
							else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
							cout<<R[I][(R_used[I][cur_instruction[I]])]<<endl;
						}
						else
						{
							pending_instruction = cur_instruction[I];
							pending_finish = req_cycle;
							pending_core = I;
						}
						cur_instruction[I]++;							//instruction will always run because first and dram is empty
					}
					else
					{
						// if below is check for wheather instrucion is runnable.
						if(cur_instruction[I] != decode_d(memory_instruction,R[I],cur_instruction[I],type,I,exit_instruction[I],busy[I],R_used[I],buffer,blocked,((1024/N)*I*1024),priority))
						{
							blocked[I] = false;
							//cout<<instruction<<endl;
							int add = address_of_instruction(memory_instruction,R[I],exit_instruction[I],((1024/N)*I*1024));
							Node* temp = new Node(cur_instruction[I]);
							temp->saved_address = add;			//save address here becasue might change due to change in Reg.value
							temp->core = I;

							if(type==9) 
							{row_updates++;	temp->data_entered = R[I][((1<<5)-1) & (memory_instruction>>21)];}		

							bool can = false;
							auto where_to_ins = head;
							int flag = 0;
							while(where_to_ins != tail && flag==0)
							{
								if(where_to_ins -> saved_address /1024 == add / 1024) can = true;
								else if(can == true) flag = 1;
								where_to_ins = where_to_ins->next;
							}
							where_to_ins = where_to_ins->prev;
							{
								temp -> prev = where_to_ins;		//insertions in dequeue.
								temp -> next = where_to_ins -> next;
								temp->next->prev = temp;
								where_to_ins->next = temp;
							}

							wait_buffer_size++;
							busy[I][R_used[I][cur_instruction[I]]].second = temp;
							cout<<"core :"<<I<<" line number "<<cur_instruction[I]<<": cycle "<<cycle<<": instruction saved in wait buffer"<<endl;
							cur_instruction[I]++;
						}
						else blocked[I] = true;
					}
				}
				else if(wait_buffer_size==MAX_WAIT_BUFFER_SIZE)
				{cout<<"core :"<<I<<" line number "<<cur_instruction[I]<<": cycle "<<cycle<<": wait buffer size full WAITING...."<<endl;}
				else if(write_port[I]==cycle)
				{cout<<"core :"<<I<<" line number "<<cur_instruction[I]<<": cycle "<<cycle<<": write port of this core busy WAITING...."<<endl;}
				else throw invalid_argument("Unexpected memory instruction");

				if(cur_instruction[I]==exit_instruction[I]) remaining_cores--;
				//cout<<req_cycle<<" "<<cycle<<endl;
			}
		}
		cycle++;
		req_cycle = max(req_cycle,cycle); //to ensure req_cycle>=cycle.
	}

	bool time_up = false;
	while(cycle<=M && !time_up)
	{
		if(req_cycle == cycle && buffer_row!=-1)
		{
			if(prev_dram_ins!=-1)		//there is actually a dram instuction just finished.
			{free(prev_dram_ins,R_used[core_in_dram],busy[core_in_dram]);prev_dram_ins = -1;}

			if(head->next->data != -1)		//common queue is not empty and buffer_row has no ins.
			{
				Node* temp = head -> next;		//selection for next row instruction.
				
				int ins = temp-> data;
				int core = temp->core;
				core_in_dram = core;

				prev_dram_ins = ins;
				int add = temp->saved_address;
				int enter_data = temp->data_entered;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				wait_buffer_size--;

				int memory_instruction = memory[ins/256 + core*(1024/N)][ins%256];
						
				if(add/1024 == buffer_row)
				{
					req_cycle = cycle + col_delay;
				}		
				else if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay;
					row_updates++;
					num_sw = 0;
				}
				else {req_cycle = cycle + row_delay+ col_delay;num_sw = 0;}

				buffer_row = add/1024;
				copy_row(memory,buffer,buffer_row);

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					pending_core = core;
					time_up = true;
					break;
				}				

				//cout<<"core :"<<<<" line number "<<ins <<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"core :"<<core_in_dram<<" line number "<<ins <<" : cycle "<<cycle<<" - "<<req_cycle-1;

				int type = (memory_instruction>>26 & ((1<<5)-1));
				if(type == 8)
				{cout<<": "<<hash[(R_used[core_in_dram][ins])]<<" = "<<R[core_in_dram][(R_used[core_in_dram][ins])]<<endl;}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = "<<enter_data<<endl;;
				

				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;
			}
			else time_up = true;
		}
		cycle = max(req_cycle,cycle+1);
	}

	cout<<endl;
	if(pending_instruction!=-1)
	{cout<<"Instruction : "<<pending_instruction<<" of core : "<<pending_core<<" has not completed in execution and will ";
	cout<<"finish at cycle : "<<pending_finish<<endl<<endl;}

	cout<<"The instructions remaining in dram which have not been executed are \n";
	int first = 0;
	while(head->next->data != -1)
	{	
		first = 1;
		Node* temp = head -> next;		//selection for next row instruction.
				
		int ins = temp-> data;
		int core = temp->core;

		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		cout<<"core :"<<core<<" line number "<<ins <<endl;
	}
	if(first==0){cout<<"NONE\n\n";}
	else cout<<endl;
	
	for(int i=0;i<N;i++)
	{
		if(cur_instruction[i]<exit_instruction[i])
		{cout<<"Remaining number of instructions in core :"<<i<<" were "<<exit_instruction[i]-cur_instruction[i]<<endl;}
		else
		{cout<<"Core : "<<i<<" completed successfully"<<endl;}
	}

	cout<<"\nTotal number of cycles : "<<min(max(cycle,req_cycle)-1,M)<<endl<<"Total number of row buffer updates : "<<row_updates<<endl;
}
