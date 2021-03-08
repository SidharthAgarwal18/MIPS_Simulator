#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
using namespace std;



void print_registers(int R[])
{
	cout<<"$zero :"<<R[0]<<endl<<"$at :"<<R[1]<<endl<<"$v0 :"<<R[2]<<endl<<"$v1 :"<<R[3]<<endl<<"$a0 :"<<R[4]<<endl;
	cout<<"$a1 :"<<R[5]<<endl<<"$a2 :"<<R[6]<<endl<<"$a3 :"<<R[7]<<endl<<"$t0 :"<<R[8]<<endl<<"$t1 :"<<R[9]<<endl;
	cout<<"$t2 :"<<R[10]<<endl<<"$t3 :"<<R[11]<<endl<<"$t4 :"<<R[12]<<endl<<"$t5 :"<<R[13]<<endl<<"$t6 :"<<R[14]<<endl;
	cout<<"$t7 :"<<R[15]<<endl<<"$s0 :"<<R[16]<<endl<<"$s1 :"<<R[17]<<endl<<"$s2 :"<<R[18]<<endl<<"$s3 :"<<R[19]<<endl;
	cout<<"$s4 :"<<R[20]<<endl<<"$s5 :"<<R[21]<<endl<<"$s6 :"<<R[22]<<endl<<"$s7 :"<<R[23]<<endl<<"$t8 :"<<R[24]<<endl;
	cout<<"$t9 :"<<R[25]<<endl<<"$k0 :"<<R[26]<<endl<<"$k1 :"<<R[27]<<endl<<"$gp :"<<R[28]<<endl<<"$sp :"<<R[29]<<endl;
	cout<<"$fp :"<<R[30]<<endl<<"$ra :"<<R[31]<<endl<<"\n";
	return ;	
}

int StringtoNumber(string name)
{
	int number = 0;
	int digit = 0;
	int pos = 0;
	int n = name.length();
	while(pos<n && name.at(pos)-'0'>=0 && name.at(pos) -'9'<=0)
	{
		digit = name.at(pos) - '0';
		number = 10*number + digit; 
		pos++;	
	}
	return number;
}


int map_instruction(string name,int instruction,int memory[])
{
	int number = 1;
	if(name.find("add")!=string::npos)number=1;
	else if(name.find("sub")!=string::npos)number=2;
	else if(name.find("mul")!=string::npos)number =3;
	else if(name.find("slt")!=string::npos)number=4;
	else if(name.find("bne")!=string::npos)number=5;
	else if(name.find("beq")!=string::npos)number=6;
	else if(name.find("j")!=string::npos)number=7;
	else if(name.find("lw")!=string::npos)number=8;
	else if(name.find("sw")!=string::npos)number=9;
	else if(name.find("addi")!=string::npos)number=10;
	else number = 11;					// 11 is for invalid instruction.
	
	memory[instruction] = number<<26;			//since only 2^31-1 max value available bits = 30
	
	
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

void type_a(string name,int memory[],int instruction)	//for add,sub,mul,slt
{
	int pos = name.find("$");
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<21);
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<16);
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<11);

	return;	
}

void type_b(string name,int memory[],int instruction)	//for beq bne addi
{
	int pos = name.find("$");
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<21);
	
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<16);
	
	pos = pos + 3;
	while(name[pos]<48 || name[pos]>57)
	{pos++;}
	int address = StringtoNumber(name.substr(pos,name.length()-pos));
	memory[instruction] = memory[instruction] + address; //address(or no. for addition)is stored in the 16 least sig. bits		
}

void type_c(string name,int memory[],int instruction)	//for j
{
	int pos = 0;
	while(name[pos]<48 || name[pos]>57)
	{pos++;}
	
	int address = StringtoNumber(name.substr(pos,name.length()-pos));
	memory[instruction] = memory[instruction] + address;       //address is stored in the 26 least significant bits 		
}

void type_d(string name,int memory[],int instruction)	//for lw and sw
{
	int offset;
	int pos = name.find("$");
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<21);
	
	pos = pos +3;
	while(pos<name.length() && (name[pos]<48 || name[pos]>57))
	{pos++;}
	
	offset = StringtoNumber(name.substr(pos,name.length()-pos)); //StringtoNumber stops as soon as it sees a non-digit charatecter
	
	pos = name.find("$",pos);
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + (R_index(reg)<<16);
	
	memory[instruction] = memory[instruction] + offset;	
}

void read_and_save_instruction(string instruction_string,int memory[],int instruction)
{
	int type = map_instruction(instruction_string,instruction,memory);
	//cout << type ;
	if(type==1||type==2||type==3||type==4) type_a(instruction_string,memory,instruction);
	else if(type==5||type==6||type==10) type_b(instruction_string,memory,instruction);
	else if (type==7)type_c(instruction_string,memory,instruction);
	else if(type==8 || type==9)type_d(instruction_string,memory,instruction);
	//else raise exception......
}

int decode_a(int memory_instruction,int R[],int instruction,int op)
{
	int r3 = ((1<<5)-1) & (memory_instruction>>11);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(op==1) {R[r1] = R[r2] + R[r3];}
	else if(op==2) R[r1] = R[r2] - R[r3];
	else if(op==3) R[r1] = R[r2] * R[r3];
	else if(op==4 && r2<r3) R[r1] = 1;
	else R[r1] = 0;
	return instruction+1;
}

int decode_b(int memory_instruction,int R[],int instruction,int op)
{
	int address = ((1<<16)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
		
	if(op==6 && r1==r2)
	{
		return address;
	}
	else if(op==5 && r1!=r2)
	{
		return address;
	}
	else if(op==10) R[r1] = R[r2] + address;
	return instruction+1;
}

 int decode_c(int memory_instruction,int R[],int instruction,int op)
{
	int address = ((1<<26)-1) & memory_instruction;
	return address;
}

int decode_d(int memory_instruction,int R[],int instruction,int op)
{
	int offset = ((1<<16)-1) & (memory_instruction);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	
	offset = offset/4; 				//as 1 int has 4 bytes....... 
	
	if(op==8 && r2 + offset<32) R[r1] = R[r2 + offset];
	else if(op==9 && r2+offset<32)R[r2+offset] = R[r1];
	//else raise exception invalid register location..........
	return instruction+1;
}

int main()
{
	int R[32]={0},memory[1<<20];
	int instruction = 0;
	int end_of_instruction = 0; 
	freopen("input.txt", "r", stdin);
	freopen("out.txt","w",stdout);
	string instruction_string;
	while(getline(cin,instruction_string))
	{
		read_and_save_instruction(instruction_string,memory,instruction);
		instruction++;
	}
	end_of_instruction = instruction;
	//cout<<" "<<instruction<<"b";
	instruction = 0;
	R[8]=9;R[9]=9;R[10] = 145;
	//cout<<"a ";
	
	print_registers(R);
	while(instruction<end_of_instruction)
	{
		int memory_instruction = memory[instruction];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		//cout<<type;
		//cout<<"a"<<type<<"a";
		if(type==1||type==2||type==3||type==4) instruction = decode_a(memory_instruction,R,instruction,type);
		else if(type==5||type==6||type==10) instruction = decode_b(memory_instruction,R,instruction,type);
		else if(type==7)instruction = decode_c(memory_instruction,R,instruction,type);
		else instruction = decode_d(memory_instruction,R,instruction,type);
		print_registers(R);
	}
	return 0;
}
