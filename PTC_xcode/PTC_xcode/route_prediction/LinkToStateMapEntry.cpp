/*
 * LinkStateMapEntry.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMapEntry.h"

namespace PredictivePowertrain {

LinkToStateMapEntry::LinkToStateMapEntry() {
    this->entries = new GenericMap<int, int>();
}

int LinkToStateMapEntry::addEntry(Link* li) {
	int linkHash = li->getHash();
	if (this->entries->hasEntry(linkHash)){
		int existingM = this->entries->getEntry(linkHash);
		this->entries->updateEntry(linkHash, existingM + 1);
        return 1;
	} else {
		this->entries->addEntry(linkHash, 1);
        return -1;
	}
}

LinkToStateMapEntry::~LinkToStateMapEntry() {
    delete(this->entries);
}

LinkToStateMapEntry::LinkToStateMapEntry(LinkToStateMapEntry& other) {
	this->entries = other.entries;
}

int LinkToStateMapEntry::getM(Link* li) {
	int linkHash = li->getHash();
	if (this->entries->hasEntry(linkHash)){
		return this->entries->getEntry(linkHash);
	} else {
		return 0;
	}
}

int LinkToStateMapEntry::getTotalM() {
	int sum = 0;
	if (this->entries->getSize() == 0){
		return 0;

	} else {
		for (auto it = this->entries->begin(); it != this->entries->end(); ++it){
			sum += it->second;
		}
		return sum;
	}
}

int LinkToStateMapEntry::totalM(LinkToStateMapEntry* linkToStateMapEntry) {
		return linkToStateMapEntry->getTotalM();
}

} /* namespace PredictivePowertrain */
