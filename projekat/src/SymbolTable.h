#pragma once
#include<string>
#include"STEntry.h"
#include<vector>

using namespace std;

class SymbolTable
{
public:
	SymbolTable();
	~SymbolTable();
	void addSybol(string name, string section, int offset, int local_global);
	void addSy(string name, string section, int offset, int local_global,string flags);
	bool exist(string name);
	int size();
	string getName(int i);
	int getOffset(int i);
	int getLocal_Global(int i);
	int getId(int i);
	string getSection(int i);
	void setLocalGlobal(string name);
	void setFlags(string flags,int i);
	string getFlags(int i);
	STEntry* getSymbol(string name);

private:
	 vector<STEntry*> table;
	 int row; // broj reda koji treba da se popuni sledeci
};

