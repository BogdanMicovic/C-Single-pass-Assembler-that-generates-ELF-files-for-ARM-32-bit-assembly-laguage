#include "LabelParser.h"
#include"Assembller.h"



LabelParser::LabelParser(LineParser* lineP) {
	this->lineP = lineP;
}



void LabelParser::parse(string line,int pass) {
	if (pass == 1) {
		int index = line.find(":");
		string label = line.substr(0, index);
		string other = "";
		bool ima_posle_labele = false;
		if (line.size() > label.size() + 1) {
			ima_posle_labele = true;
			line.erase(0, index + 2);
			int size = line.size();
		    other = line.substr(0, size);
			cout << " OTHER: " << other;
		}
		
		
		if (Assembller::symbolTable.exist(label)) {
			cout << " GRESKA,simbol" << line << " vec postoji u tabeli ";
		}
		else {
			Assembller::symbolTable.addSybol(label, Assembller::currentSection, Assembller::LC, 1);

		}
		if (ima_posle_labele) {
			this->lineP->parseLine(other, 1);
		}

	}
	else {
		int index = line.find(":");
		string label = line.substr(0, index);
		string other = "";
		bool ima_posle_labele = false;
		if (line.size() > label.size() + 1) {
			ima_posle_labele = true;
			line.erase(0, index + 2);
			int size = line.size();
			other = line.substr(0, size);
			cout << " OTHER: " << other;
		}
		if (ima_posle_labele) {
			this->lineP->parseLine(other, 2);
		}
	
	}
	
}

LabelParser::~LabelParser()
{
}
