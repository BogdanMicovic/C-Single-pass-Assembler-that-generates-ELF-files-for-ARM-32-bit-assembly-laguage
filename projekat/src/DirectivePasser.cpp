#include "DirectivePasser.h"
#include"Assembller.h"
#include"SectionEntry.h"
#include<iostream>
#include<string>
using namespace std;

const regex DirectivePasser::global("^\\.global [a-zA-Z]+[a-zA-Z0-9]*(, [a-zA-Z]+[a-zA-Z0-9]*)*");//Radi 
const regex DirectivePasser::text("^\\.text.*"); //Radi
const regex DirectivePasser::data("^\\.data.*"); //Radi
const regex DirectivePasser::bss("^\\.bss.*");//Radi
const regex DirectivePasser::word("^\\.word.*");//Radi
const regex DirectivePasser::a("^\\.align.*");//Radi - za jednocifrene vr. dec argumenata, a ne radi za 3 argumenta
const regex DirectivePasser::skip("^\\.skip.*");//Radi -apsolutni izraz jednocifrenih dec brojeva
const regex DirectivePasser::section("^\\.section.*");
const regex DirectivePasser::byte("^\\.byte.*");//Radi za izraze jednocifrenih 0x ili dec brojeva
const regex DirectivePasser::ext("^\\.extern\\s[a-zA-Z]+[a-zA-Z0-9]*(,\\s[a-zA-Z]+[a-zA-Z0-9]*)*");//Radi
const regex DirectivePasser::global_labels("^[a-zA-Z]+[a-zA-Z0-9]*,*.*");//pomocno
const regex DirectivePasser::hexa("^0x.+");//pomocno
const regex DirectivePasser::num("[0-9]+");//pomocno
const regex DirectivePasser::equ("^\\.equ.*");//Radi

// A utility function to check if a given character is operand 
bool isOperand(char c) { return (c >= '0' && c <= '9'); }

// utility function to find value of and operand 
int value(char c) { return (c - '0'); }

int evaluate(string exp)
{
	// Base Case: Given expression is empty 
	if (exp == "")  return -1;

	// The first character must be an operand, find its value 
	int res = value(exp[0]);

	// Traverse the remaining characters in pairs 
	for (int i = 1; exp[i]; i += 2)
	{
		// The next character must be an operator, and 
		// next to next an operand 
		char opr = exp[i], opd = exp[i + 1];

		// If next to next character is not an operand 
		if (!isOperand(opd))  return -1;

		// Update result according to the operator 
		if (opr == '+')       res += value(opd);
		else if (opr == '-')  res -= value(opd);
		else if (opr == '*')  res *= value(opd);
		else if (opr == '/')  res /= value(opd);

		// If not a valid operator 
		else                  return -1;
	}
	return res;
}

// Function to convert hexadecimal to decimal 
int hexadecimalToDecimal(string hexVal)
{
	int len = hexVal.size();

	// Initializing base value to 1, i.e 16^0 
	int base = 1;

	int dec_val = 0;

	// Extracting characters as digits from last character 
	for (int i = len - 1; i >= 0; i--)
	{
		// if character lies in '0'-'9', converting  
		// it to integral 0-9 by subtracting 48 from 
		// ASCII value. 
		if (hexVal[i] >= '0' && hexVal[i] <= '9')
		{
			dec_val += (hexVal[i] - 48)*base;

			// incrementing base by power 
			base = base * 16;
		}

		// if character lies in 'A'-'F' , converting  
		// it to integral 10 - 15 by subtracting 55  
		// from ASCII value 
		else if (hexVal[i] >= 'A' && hexVal[i] <= 'F')
		{
			dec_val += (hexVal[i] - 55)*base;

			// incrementing base by power 
			base = base * 16;
		}
	}

	return dec_val;
}

int operand_hexa_size(int value) {
	if (value > 4095) {
		return 4;
	}
	else {
		if (value > 256) {
			return 3;
		}
		else {
			if (value > 15) {
				return 2;
			}
			else return 1;
		}
	}
}





DirectivePasser::DirectivePasser()
{
}


DirectivePasser::~DirectivePasser()
{
}

void  DirectivePasser::parse(string line, int pass) {
	
	if (regex_match(line, DirectivePasser::text)) {
		if (pass == 1) {
			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "bss") {
				Assembller::bssLC = Assembller::LC;
			}
			Assembller::currentSection = "text";
			Assembller::LC = Assembller::textLC;
			if (!Assembller::symbolTable.exist("text")) {
				Assembller::symbolTable.addSybol("text","text",Assembller::LC,1);
			}
		
		}
		else {
			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "bss") {
				Assembller::bssLC = Assembller::LC;
			}
			Assembller::currentSection = "text";
			Assembller::LC = Assembller::textLC;
	       	
		}
	
	}

	if (regex_match(line, DirectivePasser::data)) {
		if (pass == 1) {
			if (Assembller::currentSection == "text") {
				Assembller::textLC = Assembller::LC;
			}
			if (Assembller::currentSection == "bss") {
				Assembller::bssLC = Assembller::LC;
			}
			Assembller::currentSection = "data";
			Assembller::LC = Assembller::dataLC;
			if (!Assembller::symbolTable.exist("data")) {
				Assembller::symbolTable.addSybol("data", "data", Assembller::LC, 1);
			}
		}
		else {
			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "bss") {
				Assembller::bssLC = Assembller::LC;
			}
			
			Assembller::currentSection = "data";
			Assembller::LC = Assembller::dataLC;

		}
	
	}
	

	if (regex_match(line, DirectivePasser::bss)) {
	
		if (pass == 1) {
			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "text") {
				Assembller::textLC = Assembller::LC;
			}
			Assembller::currentSection = "bss";
			Assembller::LC = Assembller::bssLC;
			if (!Assembller::symbolTable.exist("bss")) {
				Assembller::symbolTable.addSybol("bss", "bss", Assembller::LC, 1);
			}
		}
		else {

			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "text") {
				Assembller::textLC = Assembller::LC;
			}
			Assembller::currentSection = "bss";
			Assembller::LC = Assembller::bssLC;

		}
	
	}


	if (regex_match(line, DirectivePasser::global)) {
		
		if (pass == 2) {
			line.erase(0, 8);
			regex simboli("^,\\s[a-zA-Z]+[a-zA-Z0-9]*.*");
			int tmp = line.find(",");
			string name = line.substr(0, tmp);
			cout << " SIMBOL_NAME: " << name << "\n";
			line.erase(0, tmp);
			if (Assembller::symbolTable.exist(name)) {
				STEntry* ste = Assembller::symbolTable.getSymbol(name);
				if (ste->getSection() == "UND") {
					cout << " GRESKA, simbol koji je definisan u nekom drugom fajlu ne moze biti globalni simbol ";
				}
				if (ste->getLocal_Global()==1) {
					Assembller::symbolTable.setLocalGlobal(name);
				}
				else {
					cout << " GRESKA, duplo navodjenje istog simbola u direktivi global ";
				}

			}
			else {
				cout << " GRESKA, siimbol nije definisan u datom fajlu ";
			//	Assembller::symbolTable.addSybol(name, Assembller::currentSection, Assembller::LC, 2); Ne moze jer ne znamo LC
			}
			while (regex_match(line, simboli)) {
				line.erase(0, 1);
				int tmp = line.find(",");
				if (tmp >= 0) {
					string name = line.substr(1, tmp-1);
					cout << " SIMBOL_NAME: " << name << "\n";
					if (Assembller::symbolTable.exist(name)) {
						STEntry* ste = Assembller::symbolTable.getSymbol(name);
						if (ste->getSection() == "UND") {
							cout << " GRESKA, simbol koji je definisan u nekom drugom fajlu ne moze biti globalni";
						}
						if (ste->getLocal_Global() == 1) {
							Assembller::symbolTable.setLocalGlobal(name);
						}
						else {
							cout << " GRESKA, duplo navodjenje istog simbola u direktivi global ";
						}

					}
					else {
						cout << " GRESKA, siimbol nije definisan u datom fajlu ";
						//	Assembller::symbolTable.addSybol(name, Assembller::currentSection, Assembller::LC, 2); Ne moze jer ne znamo LC
					}
					line.erase(0, tmp);
				}
				else {
					int size = line.size();
					string name = line.substr(1, size);
					cout << " SIMBOL_NAME: " << name << "\n";
					if (Assembller::symbolTable.exist(name)) {
						STEntry* ste = Assembller::symbolTable.getSymbol(name);
						if (ste->getSection() == "UND") {
							cout << " GRESKA, simbol koji je definisan u nekom drugom fajlu ne moze biti globalni";
						}
						if (ste->getLocal_Global() == 1) {
							Assembller::symbolTable.setLocalGlobal(name);
						}
						else {
							cout << " GRESKA, duplo navodjenje istog simbola u direktivi global ";
						}

					}
					else {
						cout << " GRESKA, siimbol nije definisan u datom fajlu ";
						//	Assembller::symbolTable.addSybol(name, Assembller::currentSection, Assembller::LC, 2); Ne moze jer ne znamo LC
					}
				}

			}
					
				}
			}
				

	if (regex_match(line, DirectivePasser::skip)) {
		if (pass == 1) {
			string expresion = line.erase(0, 6);
			regex simbol("[a-zA-z]+");
			if (regex_match(expresion, simbol)) {

				cout << "Greska";
				exit(1);
			}
			else {
				int vrednost_izraza = evaluate(expresion);
				Assembller::LC += vrednost_izraza;
				cout << "   LC= " << Assembller::LC;
			}
		}
		
	
	
	}

	if (regex_match(line, DirectivePasser::byte)) {
		if (Assembller::currentSection != "bss") {
			if (pass == 1) {
				string expresion = line.erase(0, 6);
				regex simbol("[a-zA-z]+");
				if (regex_match(line, hexa)) {
					expresion.erase(0, 2);
					if (regex_match(expresion, simbol)) {
						cout << "GRESKA, izraz za .byte direktivu mora biti apsolutni ";

					}
					else {
						Assembller::LC += 1;
						cout << "   LC= " << Assembller::LC;
					}
				}
				else {
					if (regex_match(expresion, simbol)) {
						cout << "GRESKA, izraz za .byte mora biti apsolutni";

					}
					else {
						Assembller::LC += 1;
						cout << "   LC= " << Assembller::LC;
					}
				}
				
			}
			else {
				string expresion = line.erase(0, 6);
				regex simbol("[a-zA-z]+");
				if (regex_match(line, hexa)) {
					expresion.erase(0, 2);
					if (regex_match(expresion, simbol)) {
						cout << "GRESKA, izraz za .byte mora biti apsolutni";

					}
					else {
						int vrednost_izraza = evaluate(expresion);
						cout << "/n Vrednost izraza:  " << vrednost_izraza << "  ";
						string vr = to_string(vrednost_izraza);
						cout << "\n Vrednost_to_string: " << vr << " ";
						int rez = hexadecimalToDecimal(vr);
						if (Assembller::currentSection == "data") {
							SectionEntry* se = new SectionEntry(Assembller::LC, rez);
							Assembller::sectionData.push_back(se);
						                                         
						}
						if (Assembller::currentSection == "text") {
							SectionEntry* se = new SectionEntry(Assembller::LC, rez);
							Assembller::sectionText.push_back(se);

						}
						Assembller::LC += 1;
						cout << "   LC= " << Assembller::LC;
					}
				}
				else {
					if (regex_match(expresion, simbol)) {
						cout << "GRESKA, izraz za .byte mora biti apsolutni";

					}
					else {
						int vrednost_izraza = evaluate(expresion);
						if (Assembller::currentSection == "data") {
							SectionEntry* se = new SectionEntry(Assembller::LC, vrednost_izraza);
							Assembller::sectionData.push_back(se);

						}
						if (Assembller::currentSection == "text") {
							SectionEntry* se = new SectionEntry(Assembller::LC, vrednost_izraza);
							Assembller::sectionText.push_back(se);

						}
						Assembller::LC += 1;
						cout << "   LC= " << Assembller::LC;
					}
				}
			
			}



		}
		else {
			cout << "GRESKA, u .bss sekciji se ne moze alocirati prostor ";
		}
	
	}


	if (regex_match(line, DirectivePasser::a)) {
		int moduo;
		int fill;
		int not_more;
		bool radi = false;
		regex align_parametes("^\\.align\\s[0-9+-]+,\\s[0-9+-]+,\\s[0-9+-]+");
		if (regex_match(line, align_parametes)) {
			line.erase(0, 7);
			int tmp = line.find(",");
			string m = line.substr(0, tmp);
			line.erase(0, tmp + 1);
			tmp = line.find(",");
			string f = line.substr(0, tmp);
			line.erase(0, tmp + 1);
			tmp = line.size();
			string n = line.substr(0, tmp);
			moduo = stoi(m);
			fill = stoi(f);
			not_more = stoi(n);
		}
		else {
			line.erase(0, 7);
			int size = line.size();
			string m = line.substr(0, size);
			moduo = stoi(m);
			fill = 0;
			not_more = 0;
			radi = true;
		}
		if (pass == 2) {
			int ostatak;
			if (Assembller::LC > moduo)
				ostatak = Assembller::LC % moduo;
			else
				ostatak = moduo % Assembller::LC;

			if (ostatak <= not_more || radi == true) {
				for (int i = 0; i < ostatak; i++) {
					
					if (Assembller::currentSection == "data") {
						SectionEntry* se = new SectionEntry(Assembller::LC, fill);
						Assembller::sectionData.push_back(se);
						se = new SectionEntry(Assembller::LC, fill);
						Assembller::sectionData.push_back(se);
						Assembller::LC += 1;
						Assembller::sectionData.push_back(se);
					}
					if (Assembller::currentSection == "text") {
						SectionEntry* se = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se);
						se = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se);
						Assembller::LC += 1;
					}
					if (Assembller::currentSection == "bss") {
						SectionEntry* se = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionData.push_back(se);
						se = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionData.push_back(se);
						Assembller::LC += 1;
					}

				}
			}
		}
		else {
			int ostatak;
			if(Assembller::LC>moduo)
			ostatak = Assembller::LC % moduo;
			else
			{
				ostatak = moduo % Assembller::LC;
			}
			if (ostatak <= not_more || radi == true) {
				Assembller::LC += ostatak;
			}
		}

	}


	if (regex_match(line, DirectivePasser::section)) {
		regex flags("^,\\s[a-zA-Z]+.*");
		regex ime("^[a-zA-Z]+,.*");
		string name = "";
		string flegovi = "";
		if (pass == 1) {
			line.erase(0, 9);
			
			if (regex_match(line, ime)) {
				int tmp = line.find(",");
				name = line.substr(0, tmp);
				line.erase(0, tmp); //obrisi sve do zareza 
				while (regex_match(line,flags))
				{
					string f = line.substr(2, 1);
					flegovi += f;
					line.erase(0, 3);
				}
				if (Assembller::symbolTable.exist(name)) {
					STEntry* ste = Assembller::symbolTable.getSymbol(name);
					int redni_broj = ste->getId();
					Assembller::symbolTable.setFlags(flegovi, redni_broj);
					if (Assembller::currentSection == "data") {
						Assembller::dataLC = Assembller::LC;
					}
					if (Assembller::currentSection == "text") {
						Assembller::textLC = Assembller::LC;
					}
					if (Assembller::currentSection == "bss") {
						Assembller::bssLC = Assembller::LC;
					}
					if (name == "data") {
						Assembller::currentSection = "data";
					}
					else {
						if (name == "bss") {
							Assembller::currentSection = "bss";
						}
						else {
							if (name == "text") {
								Assembller::currentSection = "text";
							}
							else {
								Assembller::currentSection = name;
							}
						}
					
					}

					
					

				}
				else {
					Assembller::symbolTable.addSy(name, name, 0, 1, flegovi);
					if (Assembller::currentSection == "data") {
						Assembller::dataLC = Assembller::LC;
					}
					if (Assembller::currentSection == "text") {
						Assembller::textLC = Assembller::LC;
					}
					if (Assembller::currentSection == "bss") {
						Assembller::bssLC = Assembller::LC;
					}
					Assembller::currentSection = name;
				}
			}
			else {
				cout << " GRESKA, posle .section direktive mora ici ime sekcije ";
			}
		}
		else {
			if (Assembller::currentSection == "data") {
				Assembller::dataLC = Assembller::LC;
			}
			if (Assembller::currentSection == "text") {
				Assembller::textLC = Assembller::LC;
			}
			if (Assembller::currentSection == "bss") {
				Assembller::bssLC = Assembller::LC;
			}
			Assembller::currentSection = name;
		}
	}

	if (regex_match(line, DirectivePasser::ext)) {
		if (pass == 2) {
			line.erase(0, 8);
			regex simboli("^,\\s[a-zA-Z]+[a-zA-Z0-9]*.*");
			int tmp = line.find(",");
			string name = line.substr(0, tmp);
			cout << " SIMBOL_NAME: " << name<<"\n";
			line.erase(0, tmp);
			if (Assembller::symbolTable.exist(name)) {
				STEntry* ste = Assembller::symbolTable.getSymbol(name);
				if (ste->getLocal_Global() == 2) {
					cout << " GRESKA, ne moze simbol koji je definisan kao globalni da bude i externi";
				}
				if (ste->getSection() == "UND") {
				
				}
				else{
					cout << " GRESKA, labela ne moze biti definisana lokalno i istovremeno biti deklarisana kao extern ";
				}

			}
			else {
				Assembller::symbolTable.addSybol(name, "UND", -1, 2);
			}
			while (regex_match(line, simboli)) {
				line.erase(0, 1);
				int tmp = line.find(",");
				if (tmp >= 0) {
					string name = line.substr(1, tmp-1);
					cout << " SIMBOL_NAME: " << name << "\n";
					if (Assembller::symbolTable.exist(name)) {
						STEntry* ste = Assembller::symbolTable.getSymbol(name);
						if (ste->getLocal_Global() == 2) {
							cout << " GRESKA, ne moze simbol koji je definisan kao globalni da bude i externi";
						}
						if (ste->getSection() == "UND") {

						}
						else {
							cout << " GRESKA, labela ne moze biti definisana lokalno i istovremeno biti deklarisana kao extern ";
						}

					}
					else {
						Assembller::symbolTable.addSybol(name, "UND", -1, 2);
					}
					line.erase(0, tmp);
				}
				else {
					int size = line.size();
					string name = line.substr(1, size);
					cout << " SIMBOL_NAME: " << name << "\n";
					if (Assembller::symbolTable.exist(name)) {
						STEntry* ste = Assembller::symbolTable.getSymbol(name);
						if (ste->getLocal_Global() == 2) {
							cout << " GRESKA, ne moze simbol koji je definisan kao globalni da bude i externi";
						}
						if (ste->getSection() == "UND") {

						}
						else {
							cout << " GRESKA, labela ne moze biti definisana lokalno i istovremeno biti deklarisana kao extern ";
						}

					}
					else {
						Assembller::symbolTable.addSybol(name, "UND", -1, 2);
					}
				}
				
			}
		}
	}

	if (regex_match(line, DirectivePasser::word)) {
		if (Assembller::currentSection != "bss") {
			regex apsolutni_izraz("^(0|1|2|3|4|5|6|7|8|9)");
			regex apsolutni_izraz_1_operand("^[0-9]+");
			regex zarez("^[a-zA-Z0-9+\\-]+,.*");
			regex dva_operanda("[a-zA-Z0-9]+(\\+|-)[a-zA-Z0-9]+");
			regex simbol("^[a-zA-Z]+[a-zA-Z0-9]+");
			line.erase(0, 6);
			if (pass == 1) {
				//prebrojimo broj zareza da bi znali koliko ima reci od 2B
				int ret = 0;//broj bajtova koji alociramo
				while (regex_match(line, zarez)) {
					ret += 2;
					int tmp = line.find(",");
					line.erase(0, tmp + 1);//brisemo i zarez
				}
				ret += 2; //poslednji izraz koji se ne zavrsava zarezom
				Assembller::LC += ret;
			}
			else {
				string izraz = "";
				string prvi_operand = "";
				string drugi_operand = "";
				bool prvi_definisan = false;
				bool drugi_definisan = false;
				while (regex_match(line, zarez)) {
					int tmp = line.find(",");
					izraz = line.substr(0, tmp);
					line.erase(0, tmp + 1);//brisemo i zarez
					if (regex_match(izraz, dva_operanda)) {//imamo dva operanda
						int tmp = izraz.find("+");
						if (tmp < 0) {
							tmp = izraz.find("-");
						}
						string znak = izraz.substr(tmp, 1);

						prvi_operand = izraz.substr(0, tmp);
						cout << " Prvi operand je: " << prvi_operand;
						izraz.erase(0, tmp + 1);
						int size = izraz.size();
						drugi_operand = izraz.substr(0, size);
						cout << " Drugi operand je: " << drugi_operand;
						int prvi;
						int drugi;
						int ret;
						if (regex_match(prvi_operand, simbol)) {
							if (Assembller::symbolTable.exist(prvi_operand)) {
								STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
								prvi = ste->getOffset();
								prvi_operand = to_string(prvi);
								prvi_definisan = true;
							}
							else {
								prvi_definisan = false;
							}
						}
						else {
							prvi_definisan = true; // definisan je ali je apsolutni izraz
						}
						if (regex_match(drugi_operand, simbol)) {
							if (Assembller::symbolTable.exist(drugi_operand)) {
								STEntry* ste = Assembller::symbolTable.getSymbol(drugi_operand);
								drugi = ste->getOffset();
								drugi_operand = to_string(drugi);
								drugi_definisan = true;
							}
							else {
								drugi_definisan = false;
							}
							//U promenljivama tipa sring prvi_operand i drugi_operand se nalaze vrednost simbola ili apsolutne vrednosti
						}
						else {
							drugi_definisan = true; //definisan je ali je apsolutni izraz
						}
						if (prvi_definisan == true && drugi_definisan == true) {//oba operanda su definisana i postoje
							string izraz = prvi_operand + znak + drugi_operand;
							int ret = evaluate(izraz);
							if (operand_hexa_size(ret) == 1) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 2) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 3) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::LC += 2;
								}
							}
							if (operand_hexa_size(ret) == 4) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 2;
								}
							}
						}
						if (prvi_definisan == false && drugi_definisan == false)
							cout << " GRESKA, ne mogu oba simbola biti nedefinisana kod .word direktive ";

						if (prvi_definisan == false) {//prvi operand je nedefinisan pa sledi apsolutna relokacija
							int ret = stoi(drugi_operand);
							if (znak == "-")
								ret = -ret;
							//Upisi prvi operand u relokacionu tabelu za odgovarajucu sekciju(text ili data)
							Assembller::symbolTable.addSybol(prvi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
							int relocation_offset = Assembller::LC;
							STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
							int redni_broj = ste->getId();
							RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
							if (Assembller::currentSection == "text")
								Assembller::relocationText.push_back(re);
							if (Assembller::currentSection == "data")
								Assembller::relocationData.push_back(re);

							//Upisi drugi operand u odgovarajucu sekciju(text ili data)
							if (operand_hexa_size(ret) == 1) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 2) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 3) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::LC += 2;
								}
							}
							if (operand_hexa_size(ret) == 4) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 2;
								}
							}
							

						}
						if (drugi_definisan == false) {//drugi operand je nedefinisan pa sledi apsolutna relokacija
							int ret = stoi(prvi_operand);
							if (znak == "-")
								ret = -ret;
							//Upisi prvi operand u relokacionu tabelu za odgovarajucu sekciju(text ili data)
							Assembller::symbolTable.addSybol(drugi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
							int relocation_offset = Assembller::LC;
							STEntry* ste = Assembller::symbolTable.getSymbol(drugi_operand);
							int redni_broj = ste->getId();
							RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
							if (Assembller::currentSection == "text")
								Assembller::relocationText.push_back(re);
							if (Assembller::currentSection == "data")
								Assembller::relocationData.push_back(re);
							//Upisi drugi operand u odgovarajucu sekciju(text ili data)
							if (operand_hexa_size(ret) == 1) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 2) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 3) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::LC += 2;
								}
							}
							if (operand_hexa_size(ret) == 4) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 2;
								}
							}
							



						}
					}
						//***
						else {//imamo jedan operand
							if (regex_match(line, apsolutni_izraz_1_operand)) {
								prvi_operand = line;
								int ret = stoi(prvi_operand);
								if (operand_hexa_size(ret) == 1) {
									if (Assembller::currentSection == "text") {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionText.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 1;
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 1;
									}
									else {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										Assembller::LC += 1;
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionData.push_back(s1);
										Assembller::LC += 1;
									}
								}
								if (operand_hexa_size(ret) == 2) {
									if (Assembller::currentSection == "text") {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 1;
										s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 1;
									}
									else {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										Assembller::LC += 1;
										s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionData.push_back(s1);
										Assembller::LC += 1;
									}
								}
								if (operand_hexa_size(ret) == 3) {
									if (Assembller::currentSection == "text") {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 2;
									}
									else {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(s1);
										s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::LC += 2;
									}
								}
								if (operand_hexa_size(ret) == 4) {
									if (Assembller::currentSection == "text") {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionText.push_back(s1);
										Assembller::LC += 2;
									}
									else {
										SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
										Assembller::sectionData.push_back(s1);
										Assembller::LC += 2;
									}
								}
							}
							else {
								prvi_operand = line;
								if (Assembller::symbolTable.exist(prvi_operand)) {
									STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
									int prvi = ste->getOffset();
									prvi_operand = to_string(prvi);
									prvi_definisan = true;
								}
								else {
									prvi_definisan = false;
								}
								if (prvi_definisan == true) {
									//STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
									int ret = stoi(prvi_operand);
									if (operand_hexa_size(ret) == 1) {
										if (Assembller::currentSection == "text") {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionText.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 1;
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 1;
										}
										else {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											Assembller::LC += 1;
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionData.push_back(s1);
											Assembller::LC += 1;
										}
									}
									if (operand_hexa_size(ret) == 2) {
										if (Assembller::currentSection == "text") {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 1;
											s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 1;
										}
										else {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											Assembller::LC += 1;
											s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionData.push_back(s1);
											Assembller::LC += 1;
										}
									}
									if (operand_hexa_size(ret) == 3) {
										if (Assembller::currentSection == "text") {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionText.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 2;
										}
										else {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
											Assembller::sectionData.push_back(s1);
											s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::LC += 2;
										}
									}
									if (operand_hexa_size(ret) == 4) {
										if (Assembller::currentSection == "text") {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionText.push_back(s1);
											Assembller::LC += 2;
										}
										else {
											SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
											Assembller::sectionData.push_back(s1);
											Assembller::LC += 2;
										}
									}
								}
								else {
									Assembller::symbolTable.addSybol(prvi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
									int relocation_offset = Assembller::LC;
									if (Assembller::currentSection == "text") {
										SectionEntry* se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(se);
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(se);
										Assembller::LC += 1;
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(se);
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionText.push_back(se);
										Assembller::LC += 1;
									}
									if (Assembller::currentSection == "data") {
										SectionEntry* se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(se);
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(se);
										Assembller::LC += 1;
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(se);
										se = new SectionEntry(Assembller::LC, 0);
										Assembller::sectionData.push_back(se);
										Assembller::LC += 1;
									}
									STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
									int redni_broj = ste->getId();
									RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
									if (Assembller::currentSection == "text")
										Assembller::relocationText.push_back(re);
									if (Assembller::currentSection == "data")
										Assembller::relocationData.push_back(re);

								}
							}
						}
					
					

				}
				//Poslednji izraz koji se ne zavrsava zarezom
				int size = line.size();
				izraz = line.substr(0, size);
				if (regex_match(izraz, dva_operanda)) {//imamo dva operanda
					int tmp = izraz.find("+");
					if (tmp < 0) {
						tmp = izraz.find("-");
					}
					string znak = izraz.substr(tmp, 1);

					prvi_operand = izraz.substr(0, tmp);
					izraz.erase(0, tmp + 1);
					int size = izraz.size();
					drugi_operand = izraz.substr(0, size);
					int prvi;
					int drugi;
					int ret;
					if (regex_match(prvi_operand, simbol)) {
						if (Assembller::symbolTable.exist(prvi_operand)) {
							STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
							prvi = ste->getOffset();
							prvi_operand = to_string(prvi);
							prvi_definisan = true;
						}
						else {
							prvi_definisan = false;
						}
					}
					else {
						prvi_definisan = true;//definisan je ali je apsolutni izraz
					}
					if (regex_match(drugi_operand, simbol)) {
						if (Assembller::symbolTable.exist(drugi_operand)) {
							STEntry* ste = Assembller::symbolTable.getSymbol(drugi_operand);
							drugi = ste->getOffset();
							drugi_operand = to_string(drugi);
							drugi_definisan = true;
						}
						else {
							drugi_definisan = false;
						}
						//U promenljivama tipa sring prvi_operand i drugi_operand se nalaze vrednost simbola ili apsolutne vrednosti
					}
					else {
						drugi_definisan = true;//definisan je ali je apsolutni izraz
					}
					if (prvi_definisan == true && drugi_definisan == true) {//oba operanda su definisana i postoje
						string izraz = prvi_operand + znak + drugi_operand;
						cout << " IZARAZ JE: " << izraz;
						int ret = evaluate(izraz);
						cout << " PRVI OPERAND JE: " << prvi_operand;
						cout << " DRUGI OPERAND JE : " << drugi_operand;
						cout << " ZNAK JE: " << znak;
						cout << " IZRACUNAO JE: " << ret;
						if (operand_hexa_size(ret) == 1) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 2) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 3) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::LC += 2;
							}
						}
						if (operand_hexa_size(ret) == 4) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 2;
							}
						}
					}
					if (prvi_definisan == false && drugi_definisan == false)
						cout << " GRESKA, ne mogu oba simbola biti nedefinisana kod .word direktive ";

					if (prvi_definisan == false) {//prvi operand je nedefinisan pa sledi apsolutna relokacija
						int ret = stoi(drugi_operand);
						if (znak == "-")
							ret = -ret;
						//Upisi prvi operand u relokacionu tabelu za odgovarajucu sekciju(text ili data)
						Assembller::symbolTable.addSybol(prvi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
						int relocation_offset = Assembller::LC;
						STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
						int redni_broj = ste->getId();
						RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
						if (Assembller::currentSection == "text")
							Assembller::relocationText.push_back(re);
						if (Assembller::currentSection == "data")
							Assembller::relocationData.push_back(re);
						//Upisi drugi operand u odgovarajucu sekciju(text ili data)
						if (operand_hexa_size(ret) == 1) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 2) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 3) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::LC += 2;
							}
						}
						if (operand_hexa_size(ret) == 4) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 2;
							}
						}
						

					}
					if (drugi_definisan == false) {//drugi operand je nedefinisan pa sledi apsolutna relokacija
						int ret = stoi(prvi_operand);
						if (znak == "-")
							ret = -ret;
						//Upisi prvi operand u relokacionu tabelu za odgovarajucu sekciju(text ili data)
						Assembller::symbolTable.addSybol(drugi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
						int relocation_offset = Assembller::LC;
						STEntry* ste = Assembller::symbolTable.getSymbol(drugi_operand);
						int redni_broj = ste->getId();
						RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
						if (Assembller::currentSection == "text")
							Assembller::relocationText.push_back(re);
						if (Assembller::currentSection == "data")
							Assembller::relocationData.push_back(re);
						//Upisi drugi operand u odgovarajucu sekciju(text ili data)
						if (operand_hexa_size(ret) == 1) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 2) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 3) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::LC += 2;
							}
						}
						if (operand_hexa_size(ret) == 4) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 2;
							}
						}
						



					}
				}
				//***
				else {//imamo jedan operand
					if (regex_match(line, apsolutni_izraz_1_operand)) {
						prvi_operand = line;
						int ret = stoi(prvi_operand);
						if (operand_hexa_size(ret) == 1) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 2) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 1;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 1;
							}
						}
						if (operand_hexa_size(ret) == 3) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(s1);
								s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::LC += 2;
							}
						}
						if (operand_hexa_size(ret) == 4) {
							if (Assembller::currentSection == "text") {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionText.push_back(s1);
								Assembller::LC += 2;
							}
							else {
								SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
								Assembller::sectionData.push_back(s1);
								Assembller::LC += 2;
							}
						}
					}
					else {
						prvi_operand = line;
						if (Assembller::symbolTable.exist(prvi_operand)) {
							STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
							int prvi = ste->getOffset();
							prvi_operand = to_string(prvi);
							prvi_definisan = true;
						}
						else {
							prvi_definisan = false;
						}
						if (prvi_definisan == true) {
						//	STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
							int ret = stoi(prvi_operand);
							if (operand_hexa_size(ret) == 1) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 2) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 1;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 1;
								}
							}
							if (operand_hexa_size(ret) == 3) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionText.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, 0);
									Assembller::sectionData.push_back(s1);
									s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::LC += 2;
								}
							}
							if (operand_hexa_size(ret) == 4) {
								if (Assembller::currentSection == "text") {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionText.push_back(s1);
									Assembller::LC += 2;
								}
								else {
									SectionEntry* s1 = new SectionEntry(Assembller::LC, ret);
									Assembller::sectionData.push_back(s1);
									Assembller::LC += 2;
								}
							}
						}
						else {
							Assembller::symbolTable.addSybol(prvi_operand, "UND", -1, 2);//dodavanje u tabelu simbola
							int relocation_offset = Assembller::LC;
							if (Assembller::currentSection == "text") {
								SectionEntry* se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(se);
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(se);
								Assembller::LC += 1;
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(se);
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionText.push_back(se);
								Assembller::LC += 1;
							}
							if (Assembller::currentSection == "data") {
								SectionEntry* se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(se);
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(se);
								Assembller::LC += 1;
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(se);
								se = new SectionEntry(Assembller::LC, 0);
								Assembller::sectionData.push_back(se);
								Assembller::LC += 1;
							}
							STEntry* ste = Assembller::symbolTable.getSymbol(prvi_operand);
							int redni_broj = ste->getId();
							RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
							if (Assembller::currentSection == "text")
								Assembller::relocationText.push_back(re);
							if (Assembller::currentSection == "data")
								Assembller::relocationData.push_back(re);
							

						}
					}
				}

				
			}
		}
		else {
			cout << " GRESKA, ne moze se upisivati u BCC sekciju ";
		}
	
	}

	if (regex_match(line, DirectivePasser::equ)) {
		if (pass == 1) {
			line.erase(0, 5);
			string simbol = "";
			string izraz = "";
			int tmp = line.find(",");
			simbol = line.substr(0, tmp);
			line.erase(0, tmp + 1);
			int size = line.size();
			izraz = line.substr(0, size);
			int ret = evaluate(izraz);
			cout << " SIMBOL: " << simbol;
			cout << " IZRAZ: " << izraz;
			if (Assembller::symbolTable.exist(simbol)) {
				STEntry* ste = Assembller::symbolTable.getSymbol(simbol);
				ste->setOffset(ret);
			}
			else {
				Assembller::symbolTable.addSy(simbol, Assembller::currentSection, ret, 1, "");
			}
		}
		

	}


	}

