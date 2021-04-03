#pragma once
#include<string>
#include<fstream>
#include<iostream>
#include"LineParser.h"
#include"SymbolTable.h"
#include"SectionEntry.h"
#include"RelocationEntry.h"


using namespace std;

using namespace std;
class Assembller
{
public:
	Assembller();
	~Assembller();
	string inputFile;
	void firstPass(istream& inputStream);
	void secondPass(istream& inputStream);
	static string textSection;
	static string dataSection;
	static string roDataSection;
	static string bssSection;
	static SymbolTable symbolTable;
	static int textLC;
	static int dataLC;
	static int roDdataLC;
	static int bssLC;
	static int LC;
	static string currentSection;
	static vector<SectionEntry*>sectionData;
	static vector<SectionEntry*>sectionText;
	static vector<SectionEntry*>sectionBss;
	static vector<RelocationEntry*>relocationText;
	static vector<RelocationEntry*>relocationData;
	static vector<RelocationEntry*>relocationBss;
	LineParser lineParser;
private:
	string currentLine;
	//fstream inputStream;
	
	
	
};

