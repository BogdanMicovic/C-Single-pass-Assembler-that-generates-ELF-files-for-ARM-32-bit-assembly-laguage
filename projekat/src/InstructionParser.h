#pragma once
#include<regex>
#include<string>
using namespace std;


class InstructionParser 
{
public:
	InstructionParser();
	~InstructionParser();

	static const regex ALU;
	static const regex CMPTEST;
	static const regex NOT;
	static const regex PUSHPOP;
	static const regex CALL;
	static const regex IRET;
	static const regex MOV;
	 static const regex SH;
	 static const regex JMP;
	 static const regex RET;

	 static const regex regdir;
	 static const regex memdir;
	 static const regex imm;
	 static const regex regindpom;
	 static const regex pcrel;
	 static const regex regind_bez_pom;
	 void parse(string line, int pass);
	 static bool skokovi_call;
};

