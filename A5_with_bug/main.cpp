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

void print_map(std::unordered_map<int,int> const &m)//Added this functon only for debugging purposes
{
	cerr << "a";
    for (auto &pair: m) {
        cerr << "{" << pair.first << ": " << pair.second << "}\n";
    }
}


int main(int argc,char* argv[])
{
	int N,M;    					//Number of files and cycles allowed
	int row_delay,col_delay;
	if(argc<5 || argc>5)			//default values...
	{
		row_delay = 10;
		col_delay = 2;		
		N = 5;
		M = 500;
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
	for(int r=0;r<1024;r++)
	{
		for(int c=0;c<256;c++)
		{
			memory[r][c] = 0;
		}
	}

	int R[N][32] = {0};
	int busy[N][32] = {0};
	int buffer_row = -1;
	int buffer[256];
	string hash[32];

	unordered_map<int,int> row_blocking_lw[N][32];			//For next row selection
	unordered_map<int,int> row_blocking_sw[N][32];
	bool blocked[N] = {false};								//Is core 'I' blocked
	unordered_set<int> rows_involved_when_blocked[N];		//Set to store row to load..
	int priority[N] = {-1};									//Hueristic for deciding which row to load.. Is proportional to cycles it will require to run based on current file data..
	for(int i=0;i<N;i++) priority[i] = -1;

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

	int start[N] = {0};				//stores the line number of first instruction in that core
	int main_instruction[N] = {-1};
	int exit_instruction[N] = {-1};
	int instruction = -1;
	int end_of_instruction = 0;
	int prev_dram_ins = -1;
	string instruction_string;
	int instruction2 = 0;

	int pending_instruction =-1;
	int pending_finish = -1;

	vector<int> core_of_ins;			//stores core number of instruction and gives in O(1)
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
		
		start[I] = instruction2;
		while(getline(cinstrm,instruction_string))			//second reading of the file for normal instructions.
		{
			if(instruction_string.find(":") == string::npos && (is_emptyline(instruction_string)==false))	//no :
			{read_and_save_instruction(instruction_string,memory,instruction2,label);
				core_of_ins.push_back(I);		//stores the core number of instruction.
				instruction2++;}
		}
		cinstrm.close();
		end_of_instruction = instruction2;
	}

	int R_used[end_of_instruction] = {0};
	int cycle = 1;
	int req_cycle = 1;


	int cur_instruction[N];
	for(int i=0;i<N;i++)
	{
		cur_instruction[i] = main_instruction[i];
	}

	unordered_set<int> core_remaining;
	for(int I=0;I<N;I++)
	{
		core_remaining.insert(I);
	}

	while(cycle <= M && core_remaining.size()!=0)
	{
		//cerr << cycle <<" "<< blocked[0] <<endl;
		if(req_cycle == cycle && buffer_row!=-1)
		{
			if(prev_dram_ins!=-1)		//there is actually a dram instuction just finished.
			{free(prev_dram_ins,R_used,busy[(core_of_ins[prev_dram_ins])]);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;			//instruction in progress.

				int add = temp->saved_address;		//address with which it was saved.

				temp->prev->next = temp->next;		//delete node from common queue in O(1)
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;		//if sw instruction.

				req_cycle = cycle + col_delay;

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}

				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<<" - "<<req_cycle-1;	//for printing.
				int type = (memory_instruction>>26 & ((1<<5)-1));
				if(type == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[ins][(R_used[ins])]<<endl;
				if(type == 8) {row_blocking_lw[core_of_ins[ins]][R_used[ins]][buffer_row]--;}
				if(type == 9) {row_blocking_sw[core_of_ins[ins]][R_used[ins]][buffer_row]--;}
				if(empty_dram) cycle++;		//if dram was empty we did cycle-- here it gets corrected before core comes.
				empty_dram = false;
				continue;
			}
			else if(head->next->data != -1)		//common queue is not empty and buffer_row has no ins.
			{
				Node* temp = head -> next;		//selection for next row instruction.
				int temp_core = -1;
				int min_dependence = INT_MAX;
				for(int i=0;i<N;i++)
				{
					if(blocked[i]==true)
					{
						if(priority[i]<min_dependence)
						{
							min_dependence = priority[i];
							temp_core = i;
						}
					}
				}
				if(temp_core!=-1 && rows_involved_when_blocked[temp_core].size()>0)
				{
					int temp_row = *(rows_involved_when_blocked[temp_core].begin());
					temp = same_row[temp_row].front();
				}
				int ins = temp-> data;
				prev_dram_ins = ins;
				int add = temp->saved_address;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
								
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay+1;
					row_updates++;
				}
				else req_cycle = cycle + row_delay+ col_delay+1;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}				

				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;

				int type = (memory_instruction>>26 & ((1<<5)-1));
				if(type == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[ins][(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				if(type == 8) {row_blocking_lw[core_of_ins[ins]][R_used[ins]][buffer_row]--;}
				if(type == 9) {row_blocking_sw[core_of_ins[ins]][R_used[ins]][buffer_row]--;}

				if(empty_dram) cycle++;		//if dram was empty we did cycle-- here it gets corrected before core comes.
				empty_dram = false;
				continue;
			}
			else empty_dram = true;
		}
		for(int I=0;I<N;I++)		//processing each core
		{
			if(cur_instruction[I] >= main_instruction[I] && cur_instruction[I] < exit_instruction[I])	//program has not yet finished
			{
				//cerr << "B" << I ;
				int memory_instruction = memory[cur_instruction[I]/256][cur_instruction[I]%256];
				int type = ((1<<5)-1) & (memory_instruction>>26);
				int prev_instruction = cur_instruction[I];

				if(type==1 || type==2 || type==3 || type==4) cur_instruction[I] = decode_a(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,start[(core_of_ins[(cur_instruction[I])])],blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority);
				else if(type==10) cur_instruction[I] = decode_b(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,start[(core_of_ins[(cur_instruction[I])])],blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority);
				else if(type==7) cur_instruction[I] = decode_c(memory_instruction,end_of_instruction,cur_instruction[I],cycle,I,start[(core_of_ins[(cur_instruction[I])])],blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority);
				else if(type==5 || type==6) cur_instruction[I] = decode_e(memory_instruction,R[I],cur_instruction[I],type,busy[I],cycle,hash,I,start[(core_of_ins[(cur_instruction[I])])],blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority);
				else if(type == 8 || type ==9)
				{
					if(buffer_row == -1)
					{
						int add = address_of_instruction(memory_instruction,R[I],end_of_instruction);
						int row = add/1024;
						copy_row(memory,buffer,row);		
						buffer_row = row;

						req_cycle = cycle + row_delay +col_delay+1;		//req_cycle is where ins is completed.
						decode_d(memory_instruction,R[I],cur_instruction[I],type,I,end_of_instruction,busy[I],R_used,buffer,blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority);

						if(type == 8) {row_blocking_lw[I][R_used[cur_instruction[I]]][row]++;}
						if(type == 9) {row_blocking_sw[I][R_used[cur_instruction[I]]][row]++;}
						
						prev_dram_ins = cur_instruction[I];				//needed for freeing the correct instruction regs. later.
						if(req_cycle<=M)
						{	
							if(type==9) {num_sw++;row_updates++;}			//check for need for write_back

							cout<<"core :"<<(core_of_ins[(cur_instruction[I])])<<" line number "<<cur_instruction[I] - start[(core_of_ins[(cur_instruction[I])])]<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;		
							cout<<"core :"<<(core_of_ins[(cur_instruction[I])])<<" line number "<<cur_instruction[I] - start[(core_of_ins[(cur_instruction[I])])]<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;

							if((memory_instruction>>26 & ((1<<5)-1)) == 8)		//different print for lw and sw.
							{cout<<": "<<hash[(R_used[cur_instruction[I]])]<<" = ";}
							else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
							cout<<R[I][(R_used[cur_instruction[I]])]<<endl;
							if(type == 8) {row_blocking_lw[I][R_used[cur_instruction[I]]][row]--;}
							if(type == 9) {row_blocking_sw[I][R_used[cur_instruction[I]]][row]--;}
						}
						else
						{
							pending_instruction = cur_instruction[I];
							pending_finish = req_cycle;
						}
						empty_dram = false;								//dram is not empty.
						cur_instruction[I]++;							//instruction will always run because first and dram is empty
					}
					else
					{
						// if below is check for wheather instrucion is runnable.
						if(cur_instruction[I] != decode_d(memory_instruction,R[I],cur_instruction[I],type,I,end_of_instruction,busy[I],R_used,buffer,blocked,rows_involved_when_blocked[I],row_blocking_sw[I],row_blocking_lw[I],priority))
						{
							//cout<<instruction<<endl;
							int add = address_of_instruction(memory_instruction,R[I],end_of_instruction);
							Node* temp = new Node(cur_instruction[I]);
							temp->saved_address = add;			//save address here becasue might change due to change in Reg.value

							if(type==9) row_updates++;			

							temp -> prev = tail -> prev;		//insertions in dequeue.
							temp -> next = tail;
							tail->prev->next = temp;
							tail->prev = temp;

							if(type == 8) {row_blocking_lw[I][R_used[cur_instruction[I]]][add/1024]++;}
							if(type == 9) {row_blocking_sw[I][R_used[cur_instruction[I]]][add/1024]++;}

							same_row[add/1024].push(temp);		//pushed in appropriate row.
							cur_instruction[I]++;
							if(empty_dram) {cycle--;empty_dram = false;}	//if dram is empty it will be executed in the same cycle there we will do cycle++ dont worry.
						}
					}
				}
				else throw invalid_argument("Unexpected memory instruction");
				
				if(cur_instruction[I]==exit_instruction[I]) {core_remaining.erase(I);}//core finished dont run again.
				//cout<<req_cycle<<" "<<cycle<<endl;
			}
		}

		cycle++;
		req_cycle = max(req_cycle,cycle); //to ensure req_cycle>=cycle.
		
	}
	while(cycle<=M && empty_dram==false)
	{
		if(req_cycle == cycle && buffer_row!=-1)
		{
			if(prev_dram_ins!=-1)		//there is actually a dram instuction just finished.
			{free(prev_dram_ins,R_used,busy[(core_of_ins[prev_dram_ins])]);prev_dram_ins = -1;}

			if(same_row[buffer_row].size()>0)
			{
				Node* temp = same_row[buffer_row].front();
				same_row[buffer_row].pop();
				int ins = temp->data;
				prev_dram_ins = ins;			//instruction in progress.

				int add = temp->saved_address;		//address with which it was saved.

				temp->prev->next = temp->next;		//delete node from common queue in O(1)
				temp->next->prev = temp->prev;

				int memory_instruction = memory[ins/256][ins%256];
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;		//if sw instruction.

				req_cycle = cycle + col_delay;		
				
				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<<" - "<<req_cycle-1;	//for printing.
				
				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}

				if((memory_instruction>>26 & ((1<<5)-1)) == 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[ins][(R_used[ins])]<<endl;

				empty_dram = false;					//dram is busy.
				continue;
			}
			else if(head->next->data != -1)		//common queue is not empty and buffer_row has no ins.
			{
				Node* temp = head -> next;		//selection for next row instruction.
				
				int ins = temp-> data;
				prev_dram_ins = ins;
				int add = temp->saved_address;

				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;
				int memory_instruction = memory[ins/256][ins%256];
								
				if(num_sw!=0) 
				{
					write_row(memory,buffer,buffer_row);
					req_cycle = cycle + 2*row_delay+ col_delay+1;
					row_updates++;
				}
				else req_cycle = cycle + row_delay+ col_delay+1;

				buffer_row = add/1024;
				same_row[buffer_row].pop();
				copy_row(memory,buffer,buffer_row);

				if(req_cycle>M)
				{
					pending_instruction = ins;
					pending_finish = req_cycle;
					if(empty_dram) cycle++;
					empty_dram = false;
					break;
				}

				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle<< ": DRAM request issued "<<endl;
				cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<" : cycle "<<cycle+1<<" - "<<req_cycle-1;

				if((memory_instruction>>26 & ((1<<5)-1))== 8)
				{cout<<": "<<hash[(R_used[ins])]<<" = ";}
				else cout<<": memory address "<<add<<"-"<<add+3<<" = ";
				cout<<R[ins][(R_used[ins])]<<endl;

				num_sw = 0;
				if((((1<<5)-1) & (memory_instruction>>26))==9) num_sw++;

				empty_dram = false;
				continue;
			}
			else empty_dram = true;
		}
		cycle = max(req_cycle,cycle+1);
	}
	cout<<endl;
	if(pending_instruction!=-1)
	{cout<<"Instruction : "<<pending_instruction<<" of core : "<<core_of_ins[pending_instruction]<<" has not completed in execution and will ";
	cout<<"finish at cycle : "<<pending_finish<<endl<<endl;}

	cout<<"The instructions remaining in dram which have not been executed are \n";
	int first = 0;
	while(head->next->data != -1)
	{	
		first = 1;
		Node* temp = head -> next;		//selection for next row instruction.
				
		int ins = temp-> data;
		
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		cout<<"core :"<<(core_of_ins[ins])<<" line number "<<ins - start[(core_of_ins[ins])]<<endl;
	}
	if(first==0){cout<<"NONE\n\n";}
	else cout<<endl;

	for(int i=0;i<N;i++)
	{
		if(core_remaining.find(i)!=core_remaining.end())
		{cout<<"Remaining number of instructions in core :"<<i<<" were "<<exit_instruction[i]-cur_instruction[i]<<endl;}
		else
		{cout<<"Core : "<<i<<" completed successfully"<<endl;}
	}

	cout<<"\nTotal number of cycles : "<<min(max(cycle,req_cycle)-1,M)<<endl<<"Total number of row buffer updates : "<<row_updates<<endl;
}