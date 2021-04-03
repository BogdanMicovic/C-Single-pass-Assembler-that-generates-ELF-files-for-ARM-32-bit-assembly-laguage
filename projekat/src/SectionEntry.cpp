#include "SectionEntry.h"



SectionEntry::SectionEntry(int off, int val)
{
	this->offset = off;
	this->value = val;
}

void SectionEntry::setOffset(int off) {
	this->offset = off;
}
void SectionEntry::setValue(int val) {
	this->value = val;
}

int SectionEntry::getOffset() {
	return this->offset;
}

int SectionEntry::getValue() {
	return this->value;
}

SectionEntry::~SectionEntry()
{
}
