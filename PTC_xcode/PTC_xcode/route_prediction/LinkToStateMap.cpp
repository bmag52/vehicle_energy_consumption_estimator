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
	long int goalHash = gj->getHash();
	GoalMapEntry<long int, LinkToStateMapEntry*>* goalEntry;
	if(!(this->goalMap->hasEntry(goalHash))) {
		goalEntry = new GoalMapEntry<long int, LinkToStateMapEntry*>(gj);
		this->goalMap->addEntry(goalHash, goalEntry);
	} else {
		goalEntry = this->goalMap->getEntry(goalHash);
	}
    goalEntry->incrementCount();
    
	long int linkHash = lj->getHash();
	if(!goalEntry->getMap()->hasEntry(linkHash)) {
		goalEntry->addMapEntry(linkHash, new LinkToStateMapEntry);
	}
	return goalEntry->getMapEntry(linkHash)->addEntry(li);
}

float LinkToStateMap::getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar)
{
	Probability pg;
	Probability pllg;
	this->goalMap->initializeCounter();
	GenericEntry<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>* nextGoalMapEntry = this->goalMap->nextEntry();
	while(nextGoalMapEntry != NULL) { // next->key != 1
		Goal* g = nextGoalMapEntry->value->getGoal();
		if(gj->isEqual(g) || isSimilar) {
			LinkToStateMapEntry* l2Entry = nextGoalMapEntry->value->getMapEntry(lj->getHash());
			if(l2Entry != NULL)
			{
				pllg.addDenominator(l2Entry->getTotalM());
				pllg.addNumerator(l2Entry->getM(li));
			}

		}
		pg.addDenominator(nextGoalMapEntry->value->getM());
		if(g->isEqual(gj)) {
			pg.addNumerator(nextGoalMapEntry->value->getM());
		}
		nextGoalMapEntry = this->goalMap->nextEntry();
	}
	float pl = pllg.getProbability();
	//float pllg = pl * pg.getProbability();
	return pl;
}

LinkToStateMap::LinkToStateMap()
{
    this->goalMap = new GenericMap<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>();
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other)
{
    this->goalMap = other.getGoalMap()->copy();
}

LinkToStateMap::~LinkToStateMap()
{
    delete(this->goalMap);
}

GenericMap<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>* LinkToStateMap::getGoalMap()
{
    return this->goalMap;
}

} /* namespace PredictivePowertrain */
