#pragma once
#include<string>

using namespace std;

class RelocationEntry
{
public:
	RelocationEntry(int offset,string tip,int vr);
	~RelocationEntry();

	int offset;
	string tip;
	int vr;
	int getOffset();
	string getTip();
	int getVr();
};

