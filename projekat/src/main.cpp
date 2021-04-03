#include<iostream>
#include<fstream>
#include"Assembller.h"
using namespace std;

int main(int argc, char* argv[]) {
	
	ifstream input;
	ofstream output;
	input.open("ulaz.txt");
	output.open("izlaz.txt");
	cout << "poceo";
	
	if (input) {
		Assembller*  assembller = new Assembller();
		assembller->firstPass(input);
		input.close();
		input.open("ulaz.txt");
		assembller->secondPass(input);
		input.close();
		output << "#tabela simbola\n";
		cout<< "#tabela simbola\n";
		output << "ime  sekcija        vr.       vid.     rb.   flegovi\n";
		cout << "ime  sekcija       vr.      vid.     rb.   flegovi\n";
		for (int i = 0; i < Assembller::symbolTable.size(); i++) {
			
			cout << Assembller::symbolTable.getName(i) << " " << Assembller::symbolTable.getSection(i) << "           " << Assembller::symbolTable.getOffset(i) << "         " << Assembller::symbolTable.getLocal_Global(i) << "       " << Assembller::symbolTable.getId(i) <<"  "<<Assembller::symbolTable.getFlags(i)<< "\n";

			output << Assembller::symbolTable.getName(i) << " " << Assembller::symbolTable.getSection(i) <<"           " << Assembller::symbolTable.getOffset(i) << "         " << Assembller::symbolTable.getLocal_Global(i) << "       " << Assembller::symbolTable.getId(i)<< "  " << Assembller::symbolTable.getFlags(i)<<"\n";
		}
  //data sekcija
		output<< "#end\n";
		cout<< "#end\n";
		output << "\n#.data\n";
		cout << "\n#.data\n";
		
		for (int j = 0; j < Assembller::sectionData.size(); j++) {
			int value = Assembller::sectionData[j]->getValue();
			cout <<hex<< value << " ";
			output << hex<<value;
		}
	 
		output << "\n#end\n";
		cout << "\n#end\n";

		output << "#rel .data\n";
		cout << "\n#rel .data\n";
		output << "#offset" << "      " << "tip" << "         " << "vr\n";
		cout << "#offset" << "      " << "tip" << "         " << "vr\n";
		for (int j = 0; j < Assembller::relocationData.size(); j++) {
			int offset = Assembller::relocationData[j]->getOffset();
			string tip = Assembller::relocationData[j]->getTip();
			int vr = Assembller::relocationData[j]->getVr();
			output << hex << offset << "      " << tip << "         " << hex << vr;
			cout << hex << offset << "      " << tip << "         " << hex << vr;
		}
		output << "\n#end\n";
		cout << "\n#end\n";
		
		output << "\n#.text\n";
		cout<< "\n#.text\n";

		for (int j = 0; j < Assembller::sectionText.size(); j++) {
			int value = Assembller::sectionText[j]->getValue();
			cout << hex << value << " ";
			output << hex << value;
		}
		output << "\n#end\n";
		cout<< "\n#end\n";

		output << "#rel .text\n";
		cout << "\n#rel .text\n";
		output << "#offset" << "      " << "tip" << "         " << "vr\n";
		cout<< "#offset" << "      " << "tip" << "         " << "vr\n";
		for (int j = 0; j < Assembller::relocationText.size(); j++) {
			int offset = Assembller::relocationText[j]->getOffset();
			string tip = Assembller::relocationText[j]->getTip();
			int vr = Assembller::relocationText[j]->getVr();
			output<< hex << offset << "      " << tip << "         " << hex << vr;
			cout << hex << offset << "      "<<tip<< "         "<<hex<<vr;
		}
		output << "\n#end\n";
		cout<< "\n#end\n";


		output.close();
	}
	else {
		cout << " GRESKA,nepostojeci ulazni fajl ";
	}
	cin.get();
	return 0;
}