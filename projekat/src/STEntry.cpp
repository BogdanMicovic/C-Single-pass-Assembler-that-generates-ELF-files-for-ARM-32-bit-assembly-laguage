#include "STEntry.h"



void STEntry:: setSection(string section) {
	this->section = section;
}

string STEntry::getName() {
	return this->name;
}

int STEntry:: getOffset() {
	return this->offset;
}

int STEntry::getLocal_Global() {
	return local_global;
}

int STEntry::getId() {
	return this->id;
}

string STEntry::getSection() {
	return this->section;
}

STEntry::~STEntry()
{
}

void STEntry::setLocalGlobal(int local_global) {
	this->local_global = local_global;
}

void STEntry::setFlags(string flags) {
	this->flags = flags;
}

string STEntry::getFlags() {
	return this->flags;
}

void STEntry::setOffset(int offset) {
	this->offset = offset;
}
