#include "SymbolTable.h"



SymbolTable::SymbolTable()
{
	this->row = 0;
}
void SymbolTable:: addSybol(string name, string section, int offset, int local_global) {
	STEntry* novi = new STEntry(name,  section,  offset,  local_global,  (this->row)++);
	this->table.push_back(novi);
}

bool SymbolTable::exist(string name) {
	if (!table.empty()) {
		for ( int i = 0; i < table.size(); i++) {
			if (table[i]->getName() == name) return true;
		}
	}
	return false;
}

SymbolTable::~SymbolTable()
{
}
int SymbolTable::size() {
	return table.size();
}

string SymbolTable::getName(int i) {
	return table[i]->getName();
}
int SymbolTable::getOffset(int i) {
	return table[i]->getOffset();
}

int SymbolTable::getLocal_Global(int i) {
	return table[i]->getLocal_Global();
}

int SymbolTable::getId(int i) {
	return table[i]->getId();
}

string SymbolTable::getSection(int i) {
	return table[i]->getSection();
}

void SymbolTable::setLocalGlobal(string name) {
	for (int i = 0; i < table.size(); i++) {
		if (table[i]->getName() == name) {
			table[i]->setLocalGlobal(2);
		}
	}

}

void SymbolTable::setFlags(string flags,int i) {
	this->table[i]->setFlags(flags);
}

string SymbolTable::getFlags(int i) {
	return this->table[i]->getFlags();
}

void SymbolTable::addSy(string name, string section, int offset, int local_global, string flags){
	STEntry* novi = new STEntry(name, section, offset, local_global, (this->row)++);
	novi->setFlags(flags);
	this->table.push_back(novi);
}

STEntry* SymbolTable::getSymbol(string name) {
	if (exist(name)) {
		for (int i = 0; i < size(); i++) {
			if (name == table[i]->getName()) {
				return table[i];
			}
		}
	}
	else {
		return nullptr;
	}
}