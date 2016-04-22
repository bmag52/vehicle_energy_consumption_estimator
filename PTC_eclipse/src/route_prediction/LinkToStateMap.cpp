/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"

namespace PredictivePowertrain {

int LinkToStateMap::incrementTransition(Link* lj, Goal* gj, Link* li) {
	int goalHash = gj->getHash();
	int linkHash = lj->getHash();
	GenericMap<int, LinkToStateMapEntry*> * thisMap;
	if(!(this->goalMap.hashInMap(goalHash))) {
		thisMap = new GenericMap<int, LinkToStateMapEntry*>();
		this->goalMap.addEntry(goalHash, thisMap);
	} else {
		thisMap = this->goalMap.getEntry(goalHash);
	}
	if(!(thisMap->hashInMap(linkHash))) {
		LinkToStateMapEntry entry;
		thisMap->addEntry(linkHash, &entry);
	}
	LinkToStateMapEntry * thisEntry = thisMap->getEntry(linkHash);
	int m = thisEntry->addEntry(li);
	return m;
}

double LinkToStateMap::getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar) {
	Probability p_g;
	Probability p_l_lg;
	this->goalMap.initializeCounter();
	GenericEntry<int, GenericMap<int, LinkToStateMapEntry*> * >* next = this->goalMap.nextEntry();
	while(next != NULL) {
		int goalHash = gj->getHash();
		int g = next->key;
		if(goalHash == g || isSimilar) {
			GenericMap<int, LinkToStateMapEntry*> * links = next->value;
			LinkToStateMapEntry * nextLink = links->getEntry(lj->getHash());
			if(nextLink != NULL) {
				p_l_lg.addDenominator(nextLink->getTotalM());
				p_l_lg.addNumerator(nextLink->getM(li));
			}
			next = this->goalMap.nextEntry();
		}
	}
	double pl = p_l_lg.getProbability();
	return pl;
}

LinkToStateMap::LinkToStateMap(){
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other) {
}

LinkToStateMap::~LinkToStateMap() {
}

} /* namespace PredictivePowertrain */


