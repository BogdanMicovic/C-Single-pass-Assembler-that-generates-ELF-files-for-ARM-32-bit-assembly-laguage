#pragma once
using namespace std;
#include<string>
class LineParser;
class LabelParser 
{
public:
	LabelParser(LineParser * lineP);
	~LabelParser();
	void parse(string line, int pass);
	LineParser* lineP;
};

