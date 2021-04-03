#pragma once
#include<regex>
#include<string>
using namespace std;

class DirectivePasser 
{
public:
	DirectivePasser();
	~DirectivePasser();
	
	static const regex global;
	static const regex text;
	static const regex data;
	static const regex ext;
	static const regex bss;
	
	static const regex word;
	static const regex byte;
	static const regex a;
	static const regex skip;
	static const regex section;
	static const regex global_labels;
	static const regex hexa;
	static const regex num;
	static const regex equ;

	void parse(string line, int pass);
};

