#pragma once
#include<string>

using namespace std;

class STEntry
{
public:
	STEntry(string name, string section, int offset, int local_global, int id) {
		this->name = name;
		this->section = section;
		this->offset = offset;
		this->local_global = local_global;
		this->id = id;
	}
	~STEntry();

	void setSection(string section);
	string getName();
	int getOffset();
	int getLocal_Global();
	int getId();
	string getSection();
	void setLocalGlobal(int local_global);
	void setFlags(string flags);
	string getFlags();
	void setOffset(int offset);


private:
	string name; // ime labele
	string section;
	int offset;
	int local_global; // 1 za lokalne,  2 za globalne promenljive
	int id;
	string flags;
};

