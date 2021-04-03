#include "LineParser.h"
#include"Assembller.h"
#include<iostream>
using namespace std;

const regex LineParser::directive("^\\.(text|data|bss|global|word|byte|extern|align|skip|equ).*");
const regex LineParser::label("^[A-Za-z0-9_]+:.*");
const regex LineParser:: instruction("^[a-z]+.*");
const regex LineParser::section1("^\\.section\\s[a-zA-Z][a-zA-Z0-9]*(,\\sa|,\\sd|,\\se|,\\sw|,\\sx|,\\sM|,\\sS|,\\sG|,\\sT)*");


LineParser::LineParser()
{
	line = "";
	label_parser = new LabelParser(this);
	directive_parser = new DirectivePasser();
	instruction_parser = new InstructionParser();
}

void LineParser::parseLine(string line,int pass) {
	
	
	/*int rez = line.find(";");
	if (rez) {
		line = line.substr(0, rez);
	}
	*/
	if (regex_match(line, label)) {
		cout << " Ovo je labela ";
		label_parser->parse(line, pass);
		
	}
	else
		if (regex_match(line, directive)||regex_match(line,section1)) {
			cout << " Ovo je direktiva "<<line<<" ";
			directive_parser->parse(line, pass);
			
		}
		else
			if (regex_match(line, instruction)) {
				cout << " Ovo je instrukcija ";
				if (Assembller::currentSection != "text") {
					cout << " GRESKA,instrukcije se moraju nalaziti u .text sekciji, a ne u "<<Assembller::currentSection<<"  ";
					
				}
				instruction_parser->parse(line, pass);
			}
			else
				cout << " GRESKA,los unos ";

}

LineParser::~LineParser()
{
}
