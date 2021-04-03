#pragma once
class SectionEntry
{
public:
	SectionEntry(int off,int val);
	~SectionEntry();
	void setOffset(int off);
	void setValue(int val);
	int getOffset();
	int getValue();
private:
	int offset; 
	int value; 
};

