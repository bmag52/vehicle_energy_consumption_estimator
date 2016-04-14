/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"
#include "../driver_prediction/Probability.h"
#include "../map/GenericEntry.h"
#include "Goal.h"
#include "../drive_prediction/Link.h"

namespace PredictivePowertrain {

int LinkToStateMap::incrementTransition(Link* lj, Goal* gj, Link* li) {
	int goalHash = gj->getHash();
	GoalMapEntry* entry;
	if(!(this->goalMap.hashInMap(goalHash))) {
		GoalMapEntry newEntry(gj); // main problem here was that constructor was not defined to take in a goal
		entry = &newEntry;
		this->goalMap.addEntry(goalHash, entry);
	} else {
		entry = this->goalMap.getEntry(goalHash);
	}
	entry->incrementCount();

	int linkHash = lj->getHash();
	if(!this->linkMap.hashInMap(linkHash)) {
		LinkToStateMapEntry linkEntry;
		this->linkMap.addEntry(linkHash, &linkEntry);
	}
	int m = this->linkMap.getEntry(linkHash)->addEntry(li); // same thing, function was not defined here and also wasnt taking a POINTER!
	return m;
}

double LinkToStateMap::getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar) {
	// TODO getProbability
	Probability p_g;
	Probability p_l_lg;
	this->goalMap.initializeCounter();
	GenericEntry<int, GoalMapEntry*>* next = this->goalMap.nextEntry(); //is this ok syntax? (yes)
	while(next->key != -1) {
		Goal* g = next->value->getGoalPtr();
		if(gj->isEqual(g) || isSimilar) { // matlab lets you pass shit around without worrying too much about var-type. only be getting goals so far
			LinkToStateMapEntry* l2Entry = next->value->getMapEntry(lj->getHash());
			if(l2Entry != NULL) { // GenericMap returns null if value not found
				p_l_lg.addDenominator(l2Entry->getTotalM());
				p_l_lg.addNumerator(l2Entry->getM(li));
			}
			p_g.addDenominator(next->value->getM());
			if(g->isEqual(gj)) {
				p_g.addNumerator(next->value->getM());
			}
			next = this->goalMap.nextEntry();
		}
	}
	double pl = p_l_lg.getProbability();
	//double plg = pl * p_g.getProbability();
	return pl;
}

LinkToStateMap::LinkToStateMap(){
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other) {
}

LinkToStateMap::~LinkToStateMap() {
}

} /* namespace PredictivePowertrain */


