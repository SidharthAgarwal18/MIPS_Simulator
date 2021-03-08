#include<iostream.h>
using namespace std;

void print_registers(int R[])
{
	cout<<"$zero :"<<R[0]<<endl<<"$at :"<<R[1]<<endl<<"$v0 :"<<R[2]<<endl<<"$v1 :"<<R[3]<<endl<<"$a0 :"<<R[4]<<endl;
	cout<<"$a1 :"<<R[5]<<endl<<"$a2 :"<<R[6]<<endl<<"$a3 :"<<R[7]<<endl<<"$t0 :"<<R[8]<<endl<<"$t1 :"<<R[9]<<endl;
	cout<<"$t2 :"<<R[10]<<endl<<"$t3 :"<<R[11]<<endl<<"$t4 :"<<R[12]<<endl<<"$t5 :"<<R[13]<<endl<<"$t6 :"<<R[14]<<endl;
	cout<<"$t7 :"<<R[15]<<endl<<"$s0 :"<<R[16]<<endl<<"$s1 :"<<R[17]<<endl<<"$s2 :"<<R[18]<<endl<<"$s3 :"<<R[19]<<endl;
	cout<<"$s4 :"<<R[20]<<endl<<"$s5 :"<<R[21]<<endl<<"$s6 :"<<R[22]<<endl<<"$s7 :"<<R[23]<<endl<<"$t8 :"<<R[24]<<endl;
	cout<<"$t9 :"<<R[25]<<endl<<"$k0 :"<<R[26]<<endl<<"$k1 :"<<R[27]<<endl<<"$gp :"<<R[28]<<endl<<"$sp :"<<R[29]<<endl;
	cout<<"$fp :"<<R[30]<<endl<<"$ra :"<<R[31]<<endl;
	return ;	
}

int StringtoNumber(string name)
{
	int number = 0;
	itn digit = 0;
	int pos = 0;
	int n = name.length();
	while(pos<n && name.at(pos)!=' ' && name.at(pos)!='\n')
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
	if(isSubstring(name,"sub")!=-1)number=2;
	else if(isSubstring(name,"mul")!=-1)number =3;
	else if(isSubstring(name,"slt")!=-1)number=4;
	else if(isSubstring(name,"bne")!=-1)number=5;
	else if(isSubstring(name,"beq")!=-1)number=6;
	else if(isSubstring(name,"j")!=-1)number=7;
	else if(isSubstring(name,"lw")!=-1)number=8;
	else if(isSubstring(name,"sw")!=-1)number=9;
	else if(isSubstring(name,"addi")!=-1)number=10;
	
	memory[instruction] = number<<26;			//since only 2^31-1 max value available bits = 30
	return number;		
}

int R_index(string name)
{
	if(isSubstring(name,"$zero")!=-1) return 0;
	else if(isSubstring(name,"$at")!=-1) return 1;
	else if(isSubstring(name,"$v0")!=-1) return 2;
	else if(isSubstring(name,"$v1")!=-1) return 3;
	else if(isSubstring(name,"$a0")!=-1) return 4;
	else if(isSubstring(name,"$a1")!=-1) return 5;
	else if(isSubstring(name,"$a2")!=-1) return 6;
	else if(isSubstring(name,"$a3")!=-1) return 7;
	else if(isSubstring(name,"$t0")!=-1) return 8;
	else if(isSubstring(name,"$t1")!=-1) return 9;
	else if(isSubstring(name,"$t2")!=-1) return 10;
	else if(isSubstring(name,"$t3")!=-1) return 11;
	else if(isSubstring(name,"$t4")!=-1) return 12;
	else if(isSubstring(name,"$t5")!=-1) return 13;
	else if(isSubstring(name,"$t6")!=-1) return 14;
	else if(isSubstring(name,"$t7")!=-1) return 15;
	else if(isSubstring(name,"$s0")!=-1) return 16;
	else if(isSubstring(name,"$s1")!=-1) return 17;
	else if(isSubstring(name,"$s2")!=-1) return 18;
	else if(isSubstring(name,"$s3")!=-1) return 19;
	else if(isSubstring(name,"$s4")!=-1) return 20;
	else if(isSubstring(name,"$s5")!=-1) return 21;
	else if(isSubstring(name,"$s6")!=-1) return 22;
	else if(isSubstring(name,"$s7")!=-1) return 23;
	else if(isSubstring(name,"$t8")!=-1) return 24;
	else if(isSubstring(name,"$t9")!=-1) return 25;
	else if(isSubstring(name,"$k0")!=-1) return 26;
	else if(isSubstring(name,"$k1")!=-1) return 27;
	else if(isSubstring(name,"$gp")!=-1) return 28;
	else if(isSubstring(name,"$sp")!=-1) return 29;
	else if(isSubstring(name,"$fp")!=-1) return 30;
	else if(isSubstring(name,"$ra")!=-1) return 31;
}

void type_a(string name,int memory[],int instruction)	//for add,sub,mul,slt
{
	int pos = name.find("$");
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + R_index(reg,R)<<21;
	
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + R_index(reg,R)<<16;
	
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + R_index(reg,R)<<11;
	
	return;	
}

void type_b(string name,int memory[],int instruction)	//for beq bne addi
{
	int pos = name.find("$");
	string reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + R_index(reg,R)<<21;
	
	pos = name.find("$",pos+3);
	reg = name.substr(pos,3);
	memory[instruction] = memory[instruction] + R_index(reg,R)<<16;
	
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

void read_and_save_instruction(String instruction_string,int memory[],instruction)
{
	int type = map_instruction(instruction_string,instruction,memory);
	if(type==1||type==2||type==3||type==4) type_a(instruction_string,memory,instruction);
	if(type==5||type==6||type==10) type_b(instruction_string,memory,instruction);
	else type_c(instruction_string,memory,instruction);
}

int decode_a(int memory_instruction,int R[],int instruction,int op)
{
	int r3 = ((1<<5)-1) & (memory_instruction>>11);
	int r2 = ((1<<5)-1) & (memory_instruction>>16);
	int r1 = ((1<<5)-1) & (memory_instruction>>21);
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(op==1) R[r1] = R[r2] + R[r3];
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
	//int op = ((1<<5)-1) & (memory_instruction>>26);
	
	if(op==6 && r1==r2)
	{
		//goto address;       // function to jump to required instruction in memory array
		return address;
	}
	else if(op==5 && r1!=r2)
	{
		//goto address;       // function to jump to required instruction in memory array
		return address;
	}
	else if(op==10) R[r1] = R[r2] + address;
	return instruction+1;
}

void decode_c(int memory_instruction,int R[],int instruction,int op)
{
	int addresss = ((1<<26)-1) & memory[instruction];
	// goto address;	// function to jump to required instruction in memory array
	return address;
}

int main()
{
	int R[32],memory[1<<20];
	int instruction = 0;
	int end_of_instruction = 0; 
	freopen("input.txt", "r", stdin);
	String instruction_string;
	while(cin.getline(instruction_string,100))
	{
		read_and_save_instruction(instruction_string,memory,instruction);
		instruction++;
	}
	end_of_instruction = instruction;
	instruction = 0;
	while(instruction<end_of_instruction)
	{
		int instruction_string = memory[instruction];
		int type = ((1<<5)-1) & (memory_instruction>>26);
		if(type==1||type==2||type==3||type==4) decode_a(instruction_string,R,instruction,type);
		if(type==5||type==6||type==10) decode_b(instruction_string,R,instruction,type);
		else decode_c(instruction_string,R,instruction,type);
	}
	return 0;
}
