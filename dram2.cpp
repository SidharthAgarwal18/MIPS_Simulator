#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include <stdexcept>
using namespace std;

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

void type_a(string name,int memory[][256],int instruction,int pos)	//for add,slt,mult,addi
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
	
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<21);
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
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<16);
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
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<11);
	pos += 3;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos<n) throw invalid_argument("Unexpected inputa");				//no trash character allowed after instruction
}

void type_b(string name,int memory[][256],int instruction,int pos)				//for addi
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputb");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputa");
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<21);
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
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<16);
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
	{memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] +(1<<15); pos++;}
	

	int address = StringtoNumber(name.substr(pos,n-pos),instruction);
	if(address > (1<<15)) throw invalid_argument("Unexpected input "+to_string(instruction)+"address greater than 2^15-1");
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + address; //address(or no. for addition)is stored in the 15 least sig. bits		
}

void type_c(string name,int memory[][256],int instruction,int pos,unordered_map<string,int> label)
{
	int n = name.length();
	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
	
	string name_label;
	int first = pos;
	while(pos<n && (int(name[pos])!=9 && int(name[pos])!=32)) pos++;
	name_label = name.substr(first,pos-first);

	if(label.find(name_label)==label.end()) throw invalid_argument("label not found in the program");

	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + label[name_label];
}	

void type_d(string name,int memory[][256],int instruction,int pos)	//for lw and sw
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n && (int(name[pos])==9||int(name[pos])==32)) pos++;			//for ws after name of instruction
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputj");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputa");			//no valid register
	
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<21);
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
	{memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] +(1<<15); pos++;}
	
	int number_end = pos;
	while(number_end<n && name[number_end]-'0'>=0 && name[number_end] -'9'<=0) number_end++;
	if(number_end==pos || number_end==n) throw invalid_argument("Unexpected inputm");
	
	int offset = StringtoNumber(name.substr(pos,number_end-pos),instruction);
	if(offset > (1<<15)) throw invalid_argument("Unexpected input "+to_string(instruction)+" |offset| greater than 2^15");
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + offset;
	
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
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<16);
	pos += 3;
	
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos==n || name[pos]!=41) throw invalid_argument("Unexpected inputq");
	pos = pos+1;
	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	if(pos<n) throw invalid_argument("Unexpected inputa");				//no trash character allowed after instruction
		
}

void type_e(string name,int memory[][256],int instruction,int pos,unordered_map<string,int> label)
{
	int n = name.length();
	int r;
	string reg;

	while(pos<n&&(int(name[pos])==9||int(name[pos])==32)) pos++;
	
	if(pos+2>=n) throw invalid_argument("Unexpected inputb");			//substring for register should pe possible
	reg = name.substr(pos,3);
	r = R_index(reg);
	if(r==0) pos += 2;
	if(r == -1) throw invalid_argument("Unexpected inputa");
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<21);
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
	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + (r<<16);
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

	memory[instruction/256][instruction%256] = memory[instruction/256][instruction%256] + label[name_label];
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

int decode_a(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,map<int,string> hash)
{
	int r3 = ((1<<5)-1) & (memory_instruction>>11);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(busy[r3]==1 || busy[r2]==1 || busy[r1]==1 || busy[r1]==2) return instruction;			//if either of them is busy dont move forward

	if(op==1) R[r1] = R[r2] + R[r3];
	else if(op==2) R[r1] = R[r2] - R[r3];
	else if(op==3) R[r1] = R[r2]*R[r3];
	else if(op==4 && R[r2]<R[r3]) R[r1] = 1;
	else R[r1] = 0;

	cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;

	return instruction+1;
}

int decode_b(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,map<int,string> hash)
{
	int address = ((1<<15)-1) & (memory_instruction);		//address is stored in 15 bits now.
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r2]==1 || busy[r1]==1 || busy[r1]==2) return instruction;							//if either of them is busy dont move forward
	
	int sign = (memory_instruction & (1<<15));					//for dealing with negative sign
	if(sign!=0) R[r1] = R[r2] - address;
	else R[r1] = R[r2] + address;

	cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<hash[r1]<<" = "<<R[r1]<<endl;
	return instruction+1;
}

int decode_c(int memory_instruction,int end_of_instruction,int instruction,int cycle)
{
	int new_instruction = ((1<<26)-1) & (memory_instruction);
	if(new_instruction>end_of_instruction) throw invalid_argument("Unexpected output in jump statement");

	cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<"jumped to line number"<<new_instruction<<endl;
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

int decode_d(int memory_instruction,int R[],int instruction,int op,int end_of_instruction,int busy[],int R_used[],int buffer[])
{
	int offset = ((1<<15)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r2]==1 || busy[r1]==1 || (busy[r2]==2 && op==8) || (busy[r1]==2 && op==8)) return instruction;

	//busy[r2] = 1;												//design  decision.
	if(op==8)busy[r1] = 1;						//only r1 is locked for lw and permanently 
	else busy[r1] = 2;							//value of r2 can be accessed but not changed
	R_used[instruction] = r1;

	int sign = (memory_instruction & (1<<15));

	int address;
	if(sign!=0) address = R[r2] - 4*offset;
	else address = R[r2] + 4*offset;

	if(address/4>=((1<<18)) || (address/4<=end_of_instruction) || address<0) 
	{throw invalid_argument("Unexpected inputw "+to_string(instruction)+" because of either access of encoded instruction data in memory or address not in memory size");}
		
	if(op==8) take_data(buffer,R,r1,(address/4)%256,address%4)	;		        
	else if(op==9) enter_data(buffer,(address/4)%256,address%4,R[r1]) ;			
	
	return instruction+1;
}

int decode_e(int memory_instruction,int R[],int instruction,int op,int busy[],int cycle,map<int,string> hash)
{
	int next_instruction = ((1<<16)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);

	if(busy[r1]==1 || busy[r2]==1) return instruction;

	if(op==6 && R[r1]==R[r2]) 
	{
		cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<"jumped to line number"<<next_instruction<<endl;
		return next_instruction;
	}
	else if(op==5 && R[r1]!=R[r2]) 
	{
		cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<"jumped to line number"<<next_instruction<<endl;
		return next_instruction;
	}
	cout<<"line number "<<instruction<<": cycle "<<cycle<<": "<<"jumped to line number"<<instruction+1<<endl;
	return instruction+1;
}

void work_done(int memory_instruction,int busy[],int R_used[])
{
	int s;
	s = R_used[memory_instruction];		
	busy[s] = 0;
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

void delete_node(Node* target,Node* tail)
{
	if(target==tail)
	{
		tail = tail->prev;
		tail->next = NULL;
	}
	else
	{
		target->next->prev = target->prev;
		target->prev->next = target->next;
		target->prev = NULL;
		target->next = NULL;
	}
	target = tail;
	return;
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

int main(int argc,char** argv)
{
	int R[32]={0};
	int memory[1024][256]={0};
	int buffer[256] = {0};
	int busy[32] = {0};
	unordered_map<string,int> label;				//maps label to their instruction number.

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
	string name_label;

	freopen("input.txt", "r", stdin);			//first reading for reading labels
	while(getline(cin,instruction_string))
	{
		if(instruction_string.find(":") != string::npos)
		{
			name_label = return_label(instruction_string);
			if(label.find(name_label)!=label.end()) throw invalid_argument("Same label repeated");

			label[name_label] = instruction;
			memory[instruction/256][instruction%256] = (12>>26);	 //for instructions with label.
			if(name_label=="main") main_instruction = instruction;
			if(name_label=="exit") exit_instruction = instruction;
		}
		if(is_emptyline(instruction_string)==false)	instruction++;
	}
	fclose(stdin);
	if(label.find("main")==label.end() || label.find("exit")==label.end()) 
	{throw invalid_argument("Either of main or eixt label is missing");}

	freopen("input.txt","r",stdin);
	freopen("out.txt","w",stdout);
	instruction = 0;
	while(getline(cin,instruction_string))
	{
		if(instruction_string.find(":") == string::npos)
		{read_and_save_instruction(instruction_string,memory,instruction,label);}
		else if(is_emptyline(instruction_string)==true) instruction--;
		instruction++;
	}
	end_of_instruction = instruction;			//now program ends at exit label.
	instruction = 0;
	
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

				cycle++;
			}
			else if(head!=tail)
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
				cout<<"line number"<<temp->data<<" : cycles "<<cycle+1<<" - "<<max_cycle-1;
				if(memory[temp->data/256][temp->data%256]>>26 & ((1<<5)-1) == 8)
				{cout<<": "<<hash[(R_used[instruction])]<<" = ";}
				else cout<<": memory address "<<variable<<"-"<<variable+3<<" = ";
				cout<<R[(R_used[instruction])]<<endl;

				cycle++;
			}
		}

		int memory_instruction = memory[instruction/256][instruction%256];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		int address;
	
		// instruction is incremented only if "instruction" has been executed.
		if(type==1 || type==2 || type==3 || type==4) instruction = decode_a(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==10) instruction = decode_b(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if(type==7) instruction = decode_c(memory_instruction,end_of_instruction,instruction,cycle);
		else if ((type ==8 || type == 9))
		{
			temp = new Node(instruction);
			same_row[buffer_row].push(make_pair(instruction,temp));
			tail->next = temp;
			temp->prev = tail;
			tail = tail->next;
		}
		else if(type==5 || type==6) instruction = decode_e(memory_instruction,R,instruction,type,busy,cycle,hash);
		else if (type==12) instruction = instruction+1;
		
		if(instruction==exit_instruction) break;
		cycle++;	
		max_cycle = max(max_cycle,cycle);	
	}
	
	write_row(memory,buffer,buffer_row);
	cout<<endl<<"Total cycles executed : "<<max_cycle-1<<endl<<"Total buffer row updates : "<<row_updates;
	return 0;
}
