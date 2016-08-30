/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"

namespace PredictivePowertrain {

int LinkToStateMap::incrementTransition(Link* lj, Goal* gj, Link* li)
{
	int goalHash = gj->getHash();
	GoalMapEntry* entry;
	if(!(this->goalMap->hasEntry(goalHash))) {
		entry = new GoalMapEntry(gj);
		entry->incrementCount();
		this->goalMap->addEntry(goalHash, entry);
	} else {
		this->goalMap->getEntry(goalHash)->incrementCount();
	}

	int linkHash = lj->getHash();
	if(!this->linkMap->hasEntry(linkHash)) {
		LinkToStateMapEntry* linkEntry = new LinkToStateMapEntry();
		this->linkMap->addEntry(linkHash, linkEntry);
	}
	int m = this->linkMap->getEntry(linkHash)->addEntry(li);
	return m;
}

double LinkToStateMap::getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar)
{
	Probability p_g;
	Probability p_l_lg;
	this->goalMap->initializeCounter();
	GenericEntry<int, GoalMapEntry*>* next = this->goalMap->nextEntry();
	while(next != NULL) { // next->key != 1
		Goal* g = next->value->getGoalPtr();
		if(gj->isEqual(g) || isSimilar) {
			LinkToStateMapEntry* l2Entry = next->value->getMapEntry(lj->getHash());
			if(l2Entry != NULL)
			{
				p_l_lg.addDenominator(l2Entry->getTotalM());
				p_l_lg.addNumerator(l2Entry->getM(li));
			}

		}
		p_g.addDenominator(next->value->getM());
		if(g->isEqual(gj)) {
			p_g.addNumerator(next->value->getM());
		}
		next = this->goalMap->nextEntry();
	}
	double pl = p_l_lg.getProbability();
	//double plg = pl * p_g.getProbability();
	return pl;
}

LinkToStateMap::LinkToStateMap()
{
    this->linkMap = new GenericMap<int, LinkToStateMapEntry*>();
    this->goalMap = new GenericMap<int, GoalMapEntry*>();
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other)
{
    this->linkMap = other.getLinkMap()->copy();
    this->goalMap = other.getGoalMap()->copy();
}

LinkToStateMap::~LinkToStateMap()
{
    free(this->linkMap);
    free(this->goalMap);
}
    
GenericMap<int, LinkToStateMapEntry*>* LinkToStateMap::getLinkMap()
{
    return this->linkMap;
}
    
GenericMap<int, GoalMapEntry*>* LinkToStateMap::getGoalMap()
{
    return this->goalMap;
}

} /* namespace PredictivePowertrain */
