#include<iostream.h>
using namespace std;

int StringtoNumber(string name)
{
	int number = 0;
	itn digit = 0;
	int pos = 0;
	int n = name.length();
	while(pos<n)
	{
		digit = name.at(pos) - '0';
		number = 10*number + digit; 
		pos++;	
	}
	return number;
}

void map_instruction(string name,int instruction,int memory[])
{
	int number = 1;
	if(isSubstring(name,"sub")!=-1)number=2;
	else if(isSubstring(name,"mul")!=-1)number =3;
	else if(isSubstring(name,"beq")!=-1)number=4;
	else if(isSubstring(name,"bne")!=-1)number=5;
	else if(isSubstring(name,"slt")!=-1)number=6;
	else if(isSubstring(name,"j")!=-1)number=7;
	else if(isSubstring(name,"lw")!=-1)number=8;
	else if(isSubstring(name,"sw")!=-1)number=9;
	else if(isSubstring(name,"addi")!=-1)number=10;
	
	memory[instruction] = number<<26;			//since only 2^31-1 max value available bits = 30
	return;		
}

int R_index(string name,int R[])
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

void type_a(string name,int memory[],int R[],int instruction)	//for add,sub,mul
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

void type_b(string name,int memory[],int R[],int instruction)	//for beq bne
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
	memory[instruction] = memory[instruction] + address;	// address is stored in the 16 least significant bits			
}


int main()
{
	int R[32],memory[1<<20];
	int instruction = 0;
	
}
