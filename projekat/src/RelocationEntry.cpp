#include "RelocationEntry.h"



RelocationEntry::RelocationEntry(int offset, string tip, int vr)
{
	this->offset = offset;
	this->tip = tip;
	this->vr = vr;
}


RelocationEntry::~RelocationEntry()
{
}

int RelocationEntry::getOffset() {
	return this->offset;
}

string RelocationEntry::getTip() {
	return this->tip;
}

int RelocationEntry::getVr() {
	return this->vr;
}