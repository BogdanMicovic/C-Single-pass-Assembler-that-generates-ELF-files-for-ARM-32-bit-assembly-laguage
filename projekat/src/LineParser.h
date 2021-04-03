#pragma once
#include<string>
#include<regex>
#include"LabelParser.h"
#include"DirectivePasser.h"
#include"InstructionParser.h"


using namespace std;

class LineParser
{
public:
	LineParser();
	~LineParser();
	void parseLine(string line,int pass);

private:
	static const regex directive;
	static const regex label;
	static const regex instruction;
	static const regex section1;
	string line;
	LabelParser* label_parser;
	DirectivePasser* directive_parser;
	InstructionParser* instruction_parser;
};

