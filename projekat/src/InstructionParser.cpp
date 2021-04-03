#include "InstructionParser.h"
#include"Assembller.h"
#include<regex>
#include<string>

using namespace std;

const regex InstructionParser::ALU("^(add|sub|mul|div|and|or|not|xor|xchg)(b|w|)\\s.*");
const regex InstructionParser::CMPTEST("^(cmp|test)(b|w|)\\s.*");
const regex InstructionParser::NOT("^not(b|w|)\\s.*");
const regex InstructionParser::PUSHPOP("^(push|pop)(b|w|)\\s.*");
const regex InstructionParser::CALL("^call\\s.*");
const regex InstructionParser::IRET("^iret.*");
const regex InstructionParser::MOV("^(mov)(b|w|)\\s.*");
const regex InstructionParser::SH("^(shl|shr)(b|w|)\\s.*");
const regex InstructionParser::JMP("^(jmpw|jeqw|jnew|jgtw)\\s.*");
const regex InstructionParser::RET("^ret");

const regex InstructionParser::regdir("r(0|1|2|3|4|5|6|7)[hl]?");  //-Radi ali nisam u obzir uzeo PSW registar
const regex InstructionParser::memdir("([a-qs-zA-QS-Z][a-zA-Z0-9]*|\\*[0-9]+).*"); //-Uradio ali bez mogucnosti izraza
const regex InstructionParser::imm("([0-9]+|0x[0-9A-F]+|&[a-zA-Z0-9]+)"); // neposredno adresiranje i vrednosti i simbola
const regex InstructionParser::regindpom("(r(0|1|2|3|4|5|6)[hl]?\\[[0-9]+\\]|r(0|1|2|3|4|5|6)[hl]?\\[[a-zA-Z][a-zA-Z0-9]+\\])");//- bez izraza 
const regex InstructionParser::pcrel("\\$[a-zA-Z0-9]+");
const regex InstructionParser::regind_bez_pom("r(0|1|2|3|4|5|6|7)[hl]?\\[\\]");
bool InstructionParser::skokovi_call = false;

InstructionParser::InstructionParser()
{
}


InstructionParser::~InstructionParser()
{
}




int two_byte_operand_hexa_size(int value) {
	if (value > 4095 ) {
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


int operand_byte_size(int value) {
	if (value > 127 || value < -128) {
		return 2;
	}
	else {
		return 1;
	}

}

// Function to convert hexadecimal to decimal 
int hexadToDecimal(string hexVal)
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

void insert_absolute_address(int addres) {
	if (two_byte_operand_hexa_size(addres) == 1) {
		SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se1);
		se1 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se1);
		Assembller::LC += 1;
		SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se2);
		se2 = new SectionEntry(Assembller::LC, addres);
		Assembller::sectionText.push_back(se2);
		Assembller::LC += 1;
	}


	if (two_byte_operand_hexa_size(addres) == 2) {
		SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se1);
		se1 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se1);
		Assembller::LC += 1;
		SectionEntry * se2 = new SectionEntry(Assembller::LC, addres);
		Assembller::sectionText.push_back(se2);
		Assembller::LC += 1;

	}

	if (two_byte_operand_hexa_size(addres) == 3) {
		SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
		Assembller::sectionText.push_back(se1);
		SectionEntry * se2 = new SectionEntry(Assembller::LC, addres);
		Assembller::sectionText.push_back(se2);
		Assembller::LC += 2;
	}
	if (two_byte_operand_hexa_size(addres) == 4) {
		SectionEntry * se2 = new SectionEntry(Assembller::LC, addres);
		Assembller::sectionText.push_back(se2);
		Assembller::LC += 2;
	}
}
void insert_simbol_address(string simbol) {
	int relocation_offset = Assembller::LC;
	SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se1);
	se1 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se1);
	Assembller::LC += 1;
	//im_di_ad = -Assembller::LC - 1;
	SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se2);
	se2 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se2);
	Assembller::LC += 1;
	STEntry* ste = Assembller::symbolTable.getSymbol(simbol);
	int redni_broj = ste->getId();
	RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
	Assembller::relocationText.push_back(re);
}

void insert_apsolute_relocation_text_section(string simbol) {
	int relocation_offset = Assembller::LC;
	SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se1);
	se1 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se1);
	Assembller::LC += 1;
	//im_di_ad = -Assembller::LC - 1;
	SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se2);
	se2 = new SectionEntry(Assembller::LC, 0);
	Assembller::sectionText.push_back(se2);
	Assembller::LC += 1;
	STEntry* ste = Assembller::symbolTable.getSymbol(simbol);
	int redni_broj = ste->getId();
	RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
	Assembller::relocationText.push_back(re);
}

void insert_simbol_pcrel_address(string simbol) {
	//AKO POSTOJI SIMBOL U TABELI SIMBOLA
	STEntry* ste = Assembller::symbolTable.getSymbol(simbol);
	int im_di_ad = ste->getOffset();
	cout << " OFFSET= " << im_di_ad;
	im_di_ad = im_di_ad -1 -Assembller::LC;
	if (im_di_ad < 0) {
		//im_di_ad = 65536 - im_di_ad;
		im_di_ad = 32767 - im_di_ad;
	}
	cout << " IMDIADdec= " << im_di_ad;
	cout << " IMDIADhex= " <<hex<< im_di_ad;
	
// NEMA NIKAKVE RELOKACIJE JER SIMBOL POSTOJI U SMT I RAZLIKA PCJA I OFFSETA SIMBOLA JE POZNATA	
	insert_absolute_address(im_di_ad);
}

void insert_pcrel_relocation(string simbol) {
	// AKO NE POSTOJI SIMBOL U TABELI SIMBOLA
	int relocation_offset = Assembller::LC;
	int im_di_ad = 32767 - Assembller::LC - 1;
	insert_absolute_address(im_di_ad);
	STEntry* ste = Assembller::symbolTable.getSymbol(simbol);
	int redni_broj = ste->getId();
	RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_PC16", redni_broj);
	Assembller::relocationText.push_back(re);
}


void InstructionCoder(string operation, string op1, string op2, int pass) {
	
		//INSTRUKCIJE -osim  skokova
	
	string op ;
	string b_w;
	if (operation != "iret" && operation!= "ret" && operation!="halt" && operation != "int") {
		if (operation.size() == 4) {
			op = operation.substr(0, 3);
			b_w = operation.substr(3, 1);
		}
		if (operation.size() == 5) {
			op = operation.substr(0, 4);
			b_w = operation.substr(4, 1);
		}
		if (operation.size() == 3) {
			op = operation.substr(0, 2);
			b_w = operation.substr(2, 1);
		}
	}
	else {
		if (operation == "iret" ||operation=="ret" || operation == "halt" || operation == "int") {
			op = operation;
			b_w = "";
		}
		else {
			if (operation == "") {
			
			}
			else {
				cout << " GRESKA, mora se uneti neka instrukcija";
			}
		}
		
	
	}
	
		int operation_code;
		if (op == "add") {
			operation_code = 5 << 3;
		}
		if (op == "sub") {
			operation_code = 6 << 3;
		}
		if (op == "mul") {
			operation_code = 7 << 3;
		}
		if (op == "div") {
			operation_code = 8 << 3;
		}
		if (op == "and") {
			operation_code = 11 << 3;
		}
		if (op == "cmp") {
			operation_code = 9 << 3;
		}
		if (op == "shr") {
			operation_code = 16 << 3;
		}
		if (op == "shl") {
			operation_code = 15 << 3;
		}
		if (op == "mov") {
			operation_code = 4 << 3;
		}
		if (op == "not") {
			operation_code = 10 << 3;
		}
		if (op == "xor") {
			operation_code = 13 << 3;
		}
		if (op == "push") {
			operation_code = 17 << 3;
		}
		if (op == "pop") {
			operation_code = 18 << 3;
		}
		if (op == "jmp") {
			operation_code = 19 << 3;
		}
		if (op == "jeq") {
			operation_code = 20 << 3;
		}
		if (op == "jne") {
			operation_code = 21 << 3;
		}
		if (op == "jgt") {
			operation_code = 22 << 3;
		}
		if (op == "call") {
			operation_code = 23 << 3;
		}
		if (op == "iret") {
			operation_code = 25 << 3;
		}
		if (op == "ret") {
			operation_code = 24 << 3;
		}
		if (op == "halt") {
			operation_code = 1 << 3;
		}
		if (op == "or") {
			operation_code = 12 << 3;
		}
		if (op == "int") {
			operation_code = 3 << 3;
		}

		if (b_w == "w") {
			operation_code = operation_code | (1 << 2);
		}
		else {
			//ovo nije ni potrebno 
			operation_code = operation_code | (0 << 2);
		}

		if (pass == 2) {
			cout << " OP1DESCR= " << operation_code;
			SectionEntry * se = new SectionEntry(Assembller::LC, operation_code);
			Assembller::sectionText.push_back(se);
		}
		Assembller::LC += 1;
		//KODIRANJE PRVOG OPERANDA
		if (regex_match(op1, InstructionParser::regdir) && op1!="" ) {
			int op1descr = 1 << 5;
			string reg_number = op1.substr(1, 1);
			int regnumber = stoi(reg_number);
			op1descr |= (regnumber << 1);
			int size = op1.size();
			int lh;
			if (size == 3 && b_w == "w") {
				cout << " GRESKA, instrukcija je za operande velicine 2B, pa se mora koristiti ceo registar";
			}
				
			if (size == 3 && b_w == "b") {
				//kodiramo poslednji bit u zavisnosti da li je high ili low
				string l_h = op1.substr(2, 1);
				if (l_h == "l")
					lh = 0;
				else
					lh = 1;
				op1descr |= (lh);
			}
			else {
				//ako nije ni high ni low podrazumeva se da je taj bit = 0
				lh = 0;
				op1descr |= (lh);
			}

			if (pass == 2) {
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
				
			}
			Assembller::LC += 1;


		}
		if (regex_match(op1, InstructionParser::imm) && op1 != "") {
			cout << " GRESKA, odredisni operand ne moze biti neposredna vrednost";
		}

		if (regex_match(op1, InstructionParser::memdir) && op1 != "") {
			regex memdir_apsolutno("\\*[0-9]+");
			int op1descr;
			int im_di_ad;
			if (regex_match(op1, memdir_apsolutno)) {
				op1descr = 5 << 5;
				int size = op1.size();
				string imdiad = op1.substr(1, size - 1);
				im_di_ad = stoi(imdiad);

				if (pass == 2) {
					SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
					Assembller::sectionText.push_back(se);
					Assembller::LC += 1;
					cout << "\n Hexa_size= " << two_byte_operand_hexa_size(im_di_ad);
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se2);
						se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;

					}

					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}



				}
				else {
					Assembller::LC += 3;
				}
			}
			else {
				if ( ( operation == "jmpw") || (operation == "jeqw") || (operation == "jnew") || (operation == "jgtw") || (operation == "call")) {
					op1descr = 5 << 5;
					if (pass == 2) {
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						cout << " OP1DESCR= " << hex << op1descr << "\n";
					}
					Assembller::LC += 1;
					if (Assembller::symbolTable.exist(op1)) {
						if (pass == 2) {
							insert_simbol_pcrel_address(op1);
						}
						else {
							Assembller::LC += 2;
						}
					}
					else {
						if(pass==2){
							Assembller::symbolTable.addSybol(op1, "UND", -1, 2);//dodavanje u tabelu simbola
							insert_pcrel_relocation(op1);
						}
						else {
							Assembller::LC += 2;
						}
					}
				}
				else {
					//AKO NISU U PITANJU INSTRUKCIJE SKOKA ILI CALL INSTRUKCIJA
					op1descr = 5 << 5;
					if (pass == 2) {
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						cout << " OP1DESCR= " << hex << op1descr << "\n";
					}
					Assembller::LC += 1;

					if (Assembller::symbolTable.exist(op1)) {
						if (pass == 2) {
							STEntry* ste = Assembller::symbolTable.getSymbol(op1);
							int ret = ste->getOffset();
							im_di_ad = ret;
							cout << " Memdir im_di_ad= " << hex << im_di_ad << "\n";
							insert_simbol_address(op1);

						}
						else {
							Assembller::LC += 2;
						}
					}
					else {
						if (pass == 2) {
							Assembller::symbolTable.addSybol(op1, "UND", -1, 2);//dodavanje u tabelu simbola
							int relocation_offset = Assembller::LC;
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							//im_di_ad = -Assembller::LC - 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se2);
							se2 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
							STEntry* ste = Assembller::symbolTable.getSymbol(op1);
							int redni_broj = ste->getId();
							RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
							Assembller::relocationText.push_back(re);
						}
						else {
							Assembller::LC += 2;
						}

					}

				}
				
			}//kraj elsa
		}


		if (regex_match(op1, InstructionParser::regindpom) && op1 != "") {
			regex simbol("r(0|1|2|3|4|5|6|7)[hl]?\\[[a-zA-Z][a-zA-Z0-9]+\\]");
			regex apsolutna_vr("r(0|1|2|3|4|5|6|7)[hl]?\\[[0-9]+\\]");

			if (b_w == "b" && !regex_match(op1, apsolutna_vr))
				cout << " GRESKA, kada instrukcija zahteva operande velicine 1B, moraju se koristiti apsolutne vrednosti ";

			if (b_w == "b" && regex_match(op1, apsolutna_vr)) {
				if (pass == 2) {
					int op1descr = 3 << 5;
					int im_di_ad;
					string reg_number = op1.substr(1, 1);
					int regnumber = stoi(reg_number);
					cout << " Regindpom broj registra: " << regnumber;
					op1descr |= (regnumber << 1);
					string h_l = op1.substr(2, 1);
					if (h_l == "h") {
						op1descr |= 1;
					}
					SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
					Assembller::sectionText.push_back(se);
					Assembller::LC += 1;
					cout << " RegindPom OP1DESCR= " << hex << op1descr << "\n";
					int size = op1.size();
					im_di_ad = stoi(op1.substr(3, size - 3));
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}

				}
				else {
					Assembller::LC += 2;
				}

			}
			else {

				if (regex_match(op1, apsolutna_vr)) {
					if (pass == 2) {
						int op1descr = 4 << 5;
						int im_di_ad;
						string reg_number = op1.substr(1, 1);
						int regnumber = stoi(reg_number);
						op1descr |= (regnumber << 1);
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						Assembller::LC += 1;
						int size = op1.size();
						im_di_ad = stoi(op1.substr(3, size - 3));
						if (two_byte_operand_hexa_size(im_di_ad) == 1) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se2);
							se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}


						if (two_byte_operand_hexa_size(im_di_ad) == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;

						}
						if (two_byte_operand_hexa_size(im_di_ad) == 3) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						if (two_byte_operand_hexa_size(im_di_ad) == 4) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
					}
					else {
						Assembller::LC += 3;
					}

				}
				else {
					if (pass == 2) {
						int op1descr = 4 << 5;
						int im_di_ad;
						string reg_number = op1.substr(1, 1);
						int regnumber = stoi(reg_number);
						op1descr |= (regnumber << 1);
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						Assembller::LC += 1;
						int size = op1.size();
						string simbol_name = op1.substr(3, size - 4);
						cout << "REGINDPOM SIMBOL_NAME: " << simbol_name << "\n";
						if (Assembller::symbolTable.exist(simbol_name)) {
							insert_simbol_address(simbol_name);

						}
						else {
							string simbol_name = op1.substr(3, size - 4);
							Assembller::symbolTable.addSybol(simbol_name, "UND", -1, 2);//dodavanje u tabelu simbola
							insert_simbol_address(simbol_name);
						}

					}
					else {
						Assembller::LC += 3;
					}

				}



			}

		}

		if (regex_match(op1, InstructionParser::regind_bez_pom) && op1 != "") {
			if (b_w == "w" && op1.size() == 5)
				cout << " GRESKA, operand mora biti velicine 2B kod registarskog indirektnog adresiranja bez pomeraja";
			if (b_w == "b" && op1.size() == 4)
				cout << " GRESKA, operand mora biti velicine 1B kod registarskog indirektnog adresiranja bez pomeraja";
			if (pass == 2) {
				int op1descr = 2 << 5;
				string reg_number = op1.substr(1, 1);
				int regnumber = stoi(reg_number);
				cout << " Regindpom broj registra: " << regnumber;
				op1descr |= (regnumber << 1);

				if (op1.size() == 5) {
					string h_l = op1.substr(2, 1);
					if (h_l == "h") {
						op1descr |= 1;
					}
				}
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
			}

			Assembller::LC += 1;
			
		
		}
		if (regex_match(op1, InstructionParser::pcrel) && op1 != "") {
			if (b_w == "b")
				cout << " GRESKA, velicina operanda mora biti 2B kolika je i adresa ";
			int op1descr = 4 << 5;
			int im_di_ad;
			int regnumber = 7;
			op1descr |= (regnumber << 1);
			if (pass == 2) {
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
			}
			Assembller::LC += 1;
			if (pass == 2) {
				int size = op1.size();
				string simbol_name = op1.substr(1, size - 1);
				cout << "PCREL SIMBOL_NAME: " << simbol_name << "\n";

				if (Assembller::symbolTable.exist(simbol_name)) {
					insert_simbol_pcrel_address(simbol_name);
				}
				else {
					Assembller::symbolTable.addSybol(simbol_name, "UND", -1, 2);//dodavanje u tabelu simbola
					insert_pcrel_relocation(simbol_name);
				}
			
			}
			else {
				Assembller::LC += 2;
			}
		
		}
		cout << " OPERAND 2 JE:" << op2;
		//KODIRANJE DRUGOG OPERANDA
		if (regex_match(op2, InstructionParser::regdir)&& op2!="") {
			int op1descr = 1 << 5;
			string reg_number = op2.substr(1, 1);
			int regnumber = stoi(reg_number);
			op1descr |= (regnumber << 1);
			int size = op2.size();
			int lh;
			if (size == 3 && b_w == "w")
				cout << " GRESKA, instrukcija je za operande velicine 2B, pa se mora koristiti ceo registar";
			if (size == 3 && b_w == "b") {
				//kodiramo poslednji bit u zavisnosti da li je high ili low
				string l_h = op2.substr(2, 1);
				if (l_h == "l")
					lh = 0;
				else
					lh = 1;
				op1descr |= (lh);
			}
			else {
				//ako nije ni high ni low podrazumeva se da je taj bit = 0
				lh = 0;
				op1descr |= (lh);
			}

			if (pass == 2) {
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
			}
			Assembller::LC += 1;

		}

		if (regex_match(op2, InstructionParser::memdir) && op2 != "") {
			regex memdir_apsolutno("\\*[0-9]+");
			int op1descr;
			int im_di_ad;
			if (regex_match(op2, memdir_apsolutno)) {
				op1descr = 5 << 5;
				int size = op2.size();
				string imdiad = op2.substr(1, size - 1);
				im_di_ad = stoi(imdiad);

				if (pass == 2) {
					SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
					Assembller::sectionText.push_back(se);
					Assembller::LC += 1;
					cout << "\n Hexa_size= " << two_byte_operand_hexa_size(im_di_ad);
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se2);
						se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;

					}

					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}



				}
				else {
					Assembller::LC += 3;
				}
			}
			else {

				op1descr = 5 << 5;
				if (pass == 2) {
					SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
					Assembller::sectionText.push_back(se);
					cout << " OP1DESCR= " << hex << op1descr << "\n";
				}
				Assembller::LC += 1;
				

				if (Assembller::symbolTable.exist(op2)) {
					if (pass == 2) {
						insert_simbol_address(op2);
					}
					else {
						Assembller::LC += 2;
					}
				}
				else {
					if (pass == 2) {
						Assembller::symbolTable.addSybol(op2, "UND", -1, 2);//dodavanje u tabelu simbola
						int relocation_offset = Assembller::LC;
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						//im_di_ad = -Assembller::LC - 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se2);
						se2 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
						STEntry* ste = Assembller::symbolTable.getSymbol(op2);
						int redni_broj = ste->getId();
						RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
						Assembller::relocationText.push_back(re);
					}
					else {
						Assembller::LC += 2;
					}

				}

			}
		}

		if (regex_match(op2, InstructionParser::regindpom) && op2 != "") {
			regex simbol("r(0|1|2|3|4|5|6|7)[hl]?\\[[a-zA-Z][a-zA-Z0-9]+\\]");
			regex apsolutna_vr("r(0|1|2|3|4|5|6|7)[hl]?\\[[0-9]+\\]");

			if (b_w == "b" && !regex_match(op2, apsolutna_vr))
				cout << " GRESKA, kada instrukcija zahteva operande velicine 1B, moraju se koristiti apsolutne vrednosti ";

			if (b_w == "b" && regex_match(op2, apsolutna_vr)) {
				if (pass == 2) {
					int op1descr = 3 << 5;
					int im_di_ad;
					string reg_number = op2.substr(1, 1);
					int regnumber = stoi(reg_number);
					cout << " Regindpom broj registra: " << regnumber;
					op1descr |= (regnumber << 1);
					string h_l = op2.substr(2, 1);
					if (h_l == "h") {
						op1descr |= 1;
					}
					SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
					Assembller::sectionText.push_back(se);
					Assembller::LC += 1;
					cout << " RegindPom OP1DESCR= " << hex << op1descr << "\n";
					int size = op2.size();
					im_di_ad = stoi(op2.substr(3, size - 3));
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}

				}
				else {
					Assembller::LC += 2;
				}

			}
			else {

				if (regex_match(op2, apsolutna_vr)) {
					if (pass == 2) {
						int op1descr = 4 << 5;
						int im_di_ad;
						string reg_number = op2.substr(1, 1);
						int regnumber = stoi(reg_number);
						op1descr |= (regnumber << 1);
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						Assembller::LC += 1;
						int size = op2.size();
						im_di_ad = stoi(op2.substr(3, size - 3));
						if (two_byte_operand_hexa_size(im_di_ad) == 1) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se2);
							se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}


						if (two_byte_operand_hexa_size(im_di_ad) == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;

						}
						if (two_byte_operand_hexa_size(im_di_ad) == 3) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						if (two_byte_operand_hexa_size(im_di_ad) == 4) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
					}
					else {
						Assembller::LC += 3;
					}

				}
				else {
					if (pass == 2) {
						int op1descr = 4 << 5;
						int im_di_ad;
						string reg_number = op2.substr(1, 1);
						int regnumber = stoi(reg_number);
						op1descr |= (regnumber << 1);
						SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
						Assembller::sectionText.push_back(se);
						Assembller::LC += 1;
						int size = op2.size();
						string simbol_name = op2.substr(3, size - 4);
						cout << "REGINDPOM SIMBOL_NAME: " << simbol_name << "\n";
						if (Assembller::symbolTable.exist(simbol_name)) {
							insert_simbol_address(simbol_name);
						}
						else {
							string simbol_name = op2.substr(3, size - 4);
							Assembller::symbolTable.addSybol(simbol_name, "UND", -1, 2);//dodavanje u tabelu simbola
							insert_simbol_address(simbol_name);
						}

					}
					else {
						Assembller::LC += 3;
					}

				}



			}

		}

		if (regex_match(op2, InstructionParser::regind_bez_pom) && op2 != "") {
			if (b_w == "w" && op2.size() == 5)
				cout << " GRESKA, operand mora biti velicine 2B kod registarskog indirektnog adresiranja bez pomeraja";
			if (b_w == "b" && op2.size() == 4)
				cout << " GRESKA, operand mora biti velicine 1B kod registarskog indirektnog adresiranja bez pomeraja";
			if (pass == 2) {
				int op1descr = 2 << 5;
				string reg_number = op2.substr(1, 1);
				int regnumber = stoi(reg_number);
				cout << " Regindpom broj registra: " << regnumber;
				op1descr |= (regnumber << 1);
				if (op2.size() == 5) {
					string h_l = op2.substr(2, 1);
					if (h_l == "h") {
						op1descr |= 1;
					}
				}
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
			}

			Assembller::LC += 1;


		}
			
		if (regex_match(op2, InstructionParser::imm) && op2 != "") {
			int op1descr = 0;
			if (pass == 2) {//operacioni kod je 0 za neposredno adresiranje
				SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
				Assembller::sectionText.push_back(se1);
				se1 = new SectionEntry(Assembller::LC, 0);
				Assembller::sectionText.push_back(se1);
			}
			Assembller::LC += 1;
			regex decimalna_vrednost("[0-9]+");
			regex hexadecimalna_vrednost("0x[0-9A-F]+");
			regex simbol("&[a-zA-Z0-9]+");
			int im_di_ad;
			if (regex_match(op2, decimalna_vrednost)) {
				im_di_ad = stoi(op2);
				if (b_w == "w") {
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 2;
						}
						
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 2;
						}
						
					}

					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						else {
							Assembller::LC += 2;
						}

					
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						if (pass == 2) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						else {
							Assembller::LC += 2;
						}
						
					}
				}
				else {
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							
						}
						else {
							Assembller::LC += 1;
						}

					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						if (pass == 2) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 1;
						}

					}
					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						cout << " GRESKA KOD NEPOSREDNOG ADRESIRANJA OPERAND MORA BITI VELICINE 1B, A NE 2B ";
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						cout << " GRESKA KOD NEPOSREDNOG ADRESIRANJA OPERAND MORA BITI VELICINE 1B, A NE 2B ";
					}
				}
				
				
			}

			if (regex_match(op2, hexadecimalna_vrednost) ) {
				im_di_ad = hexadToDecimal(op2);
				if (b_w == "w") {
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 2;
						}

					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 2;
						}

					}

					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						else {
							Assembller::LC += 2;
						}


					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						if (pass == 2) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 2;
						}
						else {
							Assembller::LC += 2;
						}

					}
				}
				else {
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						if (pass == 2) {
							SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
							Assembller::sectionText.push_back(se1);
							se1 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se1);
							Assembller::LC += 1;

						}
						else {
							Assembller::LC += 1;
						}

					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						if (pass == 2) {
							SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
							Assembller::sectionText.push_back(se2);
							Assembller::LC += 1;
						}
						else {
							Assembller::LC += 1;
						}

					}
					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						cout << " GRESKA KOD NEPOSREDNOG ADRESIRANJA OPERAND MORA BITI VELICINE 1B, A NE 2B ";
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						cout << " GRESKA KOD NEPOSREDNOG ADRESIRANJA OPERAND MORA BITI VELICINE 1B, A NE 2B ";
					}
				}

			}
			if (regex_match(op2, simbol)) {
				int size = op2.size();
				string simb = op2.substr(1, size - 1);
				cout << " Simbol je: " << simb << "\n";
				if (pass == 2) {
					if (Assembller::symbolTable.exist(simb)) {
						cout << " SIMBOL POSTOJI\n";
						insert_simbol_address(simb);

					}
					else {
						string simb = op2.substr(1, size - 1);
						Assembller::symbolTable.addSybol(simb, "UND", -1, 2);//dodavanje u tabelu simbola
						insert_simbol_address(simb);
				}

				}
				else {
					Assembller::LC += 2;
				}
			}
			
		}
		if (regex_match(op2, InstructionParser::pcrel) && op2 != "") {
			if (b_w == "b")
				cout << " GRESKA, velicina operanda mora biti 2B kolika je i adresa ";
			int op1descr = 4 << 5;
			int im_di_ad;
			int regnumber = 7;
			op1descr |= (regnumber << 1);
			if (pass == 2) {
				SectionEntry * se = new SectionEntry(Assembller::LC, op1descr);
				Assembller::sectionText.push_back(se);
			}
			Assembller::LC += 1;
			if (pass == 2) {
				int size = op2.size();
				string simbol_name = op2.substr(1, size - 1);
				cout << " PCREL SIMBOL_NAME: " << simbol_name << "\n";

				if (Assembller::symbolTable.exist(simbol_name)) {
					STEntry* ste = Assembller::symbolTable.getSymbol(simbol_name);
					int ret = ste->getOffset();
					im_di_ad = ret;
					if (two_byte_operand_hexa_size(im_di_ad) == 1) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se2);
						se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;
					}


					if (two_byte_operand_hexa_size(im_di_ad) == 2) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						Assembller::LC += 1;
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 1;

					}

					if (two_byte_operand_hexa_size(im_di_ad) == 3) {
						SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
						Assembller::sectionText.push_back(se1);
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}
					if (two_byte_operand_hexa_size(im_di_ad) == 4) {
						SectionEntry * se2 = new SectionEntry(Assembller::LC, im_di_ad);
						Assembller::sectionText.push_back(se2);
						Assembller::LC += 2;
					}


				}
				else {
					Assembller::symbolTable.addSybol(simbol_name, "UND", -1, 2);//dodavanje u tabelu simbola
					int relocation_offset = Assembller::LC;
					SectionEntry * se1 = new SectionEntry(Assembller::LC, 0);
					Assembller::sectionText.push_back(se1);
					se1 = new SectionEntry(Assembller::LC, 0);
					Assembller::sectionText.push_back(se1);
					Assembller::LC += 1;
					//im_di_ad = -Assembller::LC - 1;
					SectionEntry * se2 = new SectionEntry(Assembller::LC, 0);
					Assembller::sectionText.push_back(se2);
					se2 = new SectionEntry(Assembller::LC, 0);
					Assembller::sectionText.push_back(se2);
					Assembller::LC += 1;
					STEntry* ste = Assembller::symbolTable.getSymbol(simbol_name);
					int redni_broj = ste->getId();
					RelocationEntry* re = new RelocationEntry(relocation_offset, "R_386_16", redni_broj);
					Assembller::relocationText.push_back(re);

				}

			}
			else {
				Assembller::LC += 2;
			}

		}

	
}

void  InstructionParser::parse(string line, int pass) {
	//if (regex_match(line, InstructionParser:: ALU) || regex_match(line, InstructionParser:: CMPTEST) || regex_match(line, InstructionParser::SH) || regex_match(line, InstructionParser::MOV));

	//regexi za odredjivanje vrste instrukcija
	regex x("^(add|sub|mul|div|and|cmp|shr|shl|mov|not|xor)(b|w)\\s.*"); //3 slova
	regex y("^or(b|w)\\s.*"); //2 slova
	regex z("^(test|xchg)(b|w)\\s.*"); //4 slova
	regex a("^(jmpw|jeqw|jnew|jgtw)\\s.*");
	regex b("^(push|pop)(b|w)\\s.*");
	regex d("^(callw)\\s.*");
	regex e("^iret|halt");
	regex f("^ret");
	regex g("^intw\\s.*");
	if (regex_match(line, x)) {
        //ARITMETICKO-LOGICKE
		//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
		string instruction = line;
		string operation = instruction.substr(0, 4);//ima nastavak b ili w pa je to ukupno 4 slova
		cout << " Operacija:" << operation << " ";
		instruction.erase(0,5);//obrise se sve do operanada
		int tmp = instruction.find(",");
		string op1 = instruction.substr(0,tmp);
		cout << " op1:" << op1 << " ";
		instruction.erase(0, tmp + 1);
		int size = instruction.size();
		string op2 = instruction.substr(0, size);
		cout << " op2:" << op2 << " ";
		InstructionCoder(operation, op1, op2, pass);

	}
	else {
		if (regex_match(line, y)) {
			//OR
			//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
			string instruction = line;
			string operation = instruction.substr(0, 3);//ima nastavak b ili w pa je to ukupno 4 slova
			cout << " Operacija:" << operation << " ";
			instruction.erase(0, 4);//obrise se sve do operanada
			int tmp = instruction.find(",");
			string op1 = instruction.substr(0, tmp);
			cout << " op1:" << op1 << " ";
			instruction.erase(0, tmp + 1);
			int size = instruction.size();
			string op2 = instruction.substr(0, size);
			cout << " op2:" << op2 << " ";
			InstructionCoder(operation, op1, op2, pass);
		}
		else {
			if (regex_match(line, z)) {
				//TEST i XCHG
				//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
				string instruction = line;
				string operation = instruction.substr(0, 5);//ima nastavak b ili w pa je to ukupno 4 slova
				cout << " Operacija:" << operation << " ";
				instruction.erase(0, 6);//obrise se sve do operanada
				int tmp = instruction.find(",");
				string op1 = instruction.substr(0, tmp);
				cout << " op1:" << op1 << " ";
				instruction.erase(0, tmp + 1);
				int size = instruction.size();
				string op2 = instruction.substr(0, size);
				cout << " op2:" << op2 << " ";
				InstructionCoder(operation, op1, op2, pass);
			}
			else {
				if (regex_match(line, b)) {
					//PUSH I POP
					regex push("push.*");
					regex pop("pop.*");
					if (regex_match(line, push)) {
						//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
						string instruction = line;
						string operation = instruction.substr(0, 5);//ima nastavak b ili w pa je to ukupno 4 slova
						cout << " Operacija:" << operation << " ";
						instruction.erase(0, 6);//obrise se sve do operanada
						int tmp = instruction.size();
						string op1 = instruction.substr(0, tmp);
						cout << " op1:" << op1 << " ";

						string op2 = "";
						cout << " op2:" << op2 << " ";
						InstructionCoder(operation, op1, op2, pass);
					}
					else {
						//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
						string instruction = line;
						string operation = instruction.substr(0, 4);//ima nastavak b ili w pa je to ukupno 4 slova
						cout << " Operacija:" << operation << " ";
						instruction.erase(0, 5);//obrise se sve do operanada
						int tmp = instruction.size();
						string op1 = instruction.substr(0, tmp);
						cout << " op1:" << op1 << " ";

						string op2 = "";
						cout << " op2:" << op2 << " ";
						InstructionCoder(operation, op1, op2, pass);
					}

				}
				else
				{
					if (regex_match(line, d)) {
						//CALL
						//odredjivanje operanada a i b koje cemo koristiti da vidimo koja vrsta adresiranja je u pitanju
						string instruction = line;
						string operation = instruction.substr(0, 5);//ima nastavak b ili w pa je to ukupno 4 slova
						cout << " Operacija:" << operation << " ";
						instruction.erase(0, 6);//obrise se sve do operanada
						int tmp = instruction.size();
						string op1 = instruction.substr(0, tmp);
						cout << " op1:" << op1 << " ";

						string op2 = "";
						cout << " op2:" << op2 << " ";
						InstructionCoder(operation, op1, op2, pass);

					}
					else {
						if (regex_match(line, e)) {
							//IRET
							string instruction = line;
							string operation = instruction.substr(0, 5);
							cout << " Operacija:" << operation << " ";
							string op1 = "";
							string op2 = "";
							InstructionCoder(operation, op1, op2, pass);
						}
						else {
							if (regex_match(line, f)) {
							//RET
								string instruction = line;
								string operation = instruction.substr(0, 3);//ima nastavak b ili w pa je to ukupno 4 slova
								cout << " Operacija:" << operation << " ";
								string op1 = "";
								string op2 = "";
								InstructionCoder(operation, op1, op2, pass);
							}
							else {
								if (regex_match(line, g)) {
								//INT
									string instruction = line;
									string operation = instruction.substr(0, 3);
									cout << " Operacija:" << operation << " ";
									instruction.erase(0, 4);//obrise se sve do operanada
									int tmp = instruction.size();
									string op1 = instruction.substr(0, tmp);
									cout << " op1:" << op1 << " ";
									string op2 = "";
									cout << " op2:" << op2 << " ";
									op2 = op1;
									op1 = "";
									InstructionCoder(operation, op1, op2, pass);
								}
								else {
									if (regex_match(line, a)) {
										string instruction = line;
										string operation = instruction.substr(0, 4);
										cout << " Operacija:" << operation << " ";
										instruction.erase(0, 5);//obrise se sve do operanada
										int tmp = instruction.size();
										string op1 = instruction.substr(0, tmp);
										cout << " op1:" << op1 << " ";
										string op2 = "";
										cout << " op2:" << op2 << " ";
										InstructionCoder(operation, op1, op2, pass);
									}
									else {
										cout << " GRESKA instrukcija: " << line << " NE ODGOVARA NI JEDNOJ INSTRUKCIJI ";
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	
	

	
	


}