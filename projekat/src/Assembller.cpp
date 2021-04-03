#include "Assembller.h"

string Assembller::textSection = "text";
string Assembller::dataSection = "data";
string Assembller::roDataSection = "rodata";
string Assembller::bssSection = "bss";

int Assembller::textLC = 0;
int Assembller::dataLC = 0;
int Assembller::roDdataLC = 0;
int Assembller::bssLC = 0;

string Assembller::currentSection = "";
int Assembller::LC = 0;

SymbolTable Assembller::symbolTable;

vector<SectionEntry*>Assembller::sectionText;
vector<SectionEntry*>Assembller::sectionData;
vector<SectionEntry*>Assembller::sectionBss;

vector<RelocationEntry*>Assembller::relocationText;
vector<RelocationEntry*>Assembller::relocationData;
vector<RelocationEntry*>Assembller::relocationBss;



Assembller::Assembller()
{
	//this->inputFile = inputFIle;
	currentLine = "";
	//this->inputStream.open(inputFIle);
}


Assembller::~Assembller()
{
}


void Assembller:: firstPass(istream& inputStream) {
	cout << " PRVI PROLAZ\n";
	currentLine = "";
	getline(inputStream, currentLine);
	cout << currentLine;
	while (currentLine != ".end") {
		cout <<"PROCITAO JE "<< currentLine << "\n";
		lineParser.parseLine(currentLine,1);
		getline(inputStream, currentLine);
		cout << "\n";
	}
	//inputStream.close();
}

void Assembller::secondPass(istream& inputStream) {
	//inputStream.open(this->inputFile);
	cout << " DRUGI PROLAZ\n";
	currentLine = "";
	Assembller::bssLC = 0;
	Assembller::dataLC = 0;
	Assembller::textLC = 0;
	Assembller::LC = 0;
	getline(inputStream, currentLine);
	while (currentLine != ".end") {
		cout << "PROCITAO JE " << currentLine << "\n";
		lineParser.parseLine(currentLine, 2);
		getline(inputStream, currentLine);
		cout << "\n";
	}
	//inputStream.close();
}


