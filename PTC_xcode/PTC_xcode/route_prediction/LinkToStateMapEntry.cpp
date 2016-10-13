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
	long int linkHash = li->getHash();
	if (this->entries->hasEntry(linkHash)){
		int incrementedM = this->entries->getEntry(linkHash) + 1;
		this->entries->updateEntry(linkHash, incrementedM);
        return incrementedM;
	} else {
		this->entries->addEntry(linkHash, 1);
        return 1;
	}
}

LinkToStateMapEntry::~LinkToStateMapEntry() {
    delete(this->entries);
}

LinkToStateMapEntry::LinkToStateMapEntry(LinkToStateMapEntry& other) {
	this->entries = other.entries;
}

int LinkToStateMapEntry::getM(Link* li) {
	long int linkHash = li->getHash();
	if (this->entries->hasEntry(linkHash)){
		return this->entries->getEntry(linkHash);
	} else {
		return 0;
	}
}

int LinkToStateMapEntry::getTotalM() {
	if (this->entries->getSize() == 0)
    {
		return 0;
	}
    else
    {
        int sum = 0;
		for (auto it = this->entries->begin(); it != this->entries->end(); ++it)
        {
			sum += it->second;
		}
		return sum;
	}
}

int LinkToStateMapEntry::totalM(LinkToStateMapEntry* linkToStateMapEntry) {
		return linkToStateMapEntry->getTotalM();
}

} /* namespace PredictivePowertrain */
