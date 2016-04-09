/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"

namespace PredictivePowertrain {

int LinkToStateMap::incrementTransition(Link* link1, Goal* goal, Link* link2) {
//	int goalHash = goal->getHash();
//	GoalMapEntry * entry;
//	if(!(this->goalMap.hashInMap(goalHash))) {
//		entry = GoalMapEntry(goal);
//		this->goalMap.addEntry(goalHash, &entry);
//	} else {
//		entry = this->goalMap.getEntry(goalHash);
//	}
//	entry->increment_count();
//	int linkHash = link1->getHash();
//	if(!(this->linkMap.hashInMap(linkHash))) {
//		LinkToStateMapEntry linkEntry();
//		this->linkMap.addEntry(linkHash, &linkEntry);
//	}
//	int m = this->linkMap.getEntry(linkHash).addEntry(link2);
//	return m;
}

double LinkToStateMap::getProbability(Link* link1, Link* link2, Goal* goal, bool isSimilar) {
//	// TODO getProbability
//	Probability p_g();
//	Probability p_l_lg();
//	this->maps.initializeCounter();
//	GenericEntry<int, GoalMapEntry*> * next = this->goalMap.nextEntry(); //is this ok syntax?
//	while(next->key != -1) {
//		Goal * g = next->value.goal;
//		if(goal->isEqual(g) || goal->isSimilar(g)) { // not sure what to do about second half of test
//			LinkStateMapEntry * l2Entry = next->value.maps.getEntry(link2->getHash());
//			if(l2Entry != 0) {// is this the right test?
//				p_l_lg.addDenominator(l2Entry->get_total_m());
//				p_l_lg.addNumerator(l2Entry->get_m(*link1));
//			}
//			p_g.addDenominator(next->value.m);
//			if(g->isEqual(goal)) {
//				p_g.addNumerator(next->value.m);
//			}
//			next = this->goalMap.nextEntry();
//		}
//	}
//	double pl = p_l_lg.getProbability();
//	//double plg = pl * p_g.getProbability();
//	return pl;
}

LinkToStateMap::LinkToStateMap(){
//	GenericMap newMap();
//	this->maps = newMap;
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other) {
//	GenericMap * copy = other->copy();
//	this->maps = *copy;
}

LinkToStateMap::~LinkToStateMap() {
}

} /* namespace PredictivePowertrain */


