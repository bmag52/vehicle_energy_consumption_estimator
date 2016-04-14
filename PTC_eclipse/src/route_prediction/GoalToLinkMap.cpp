/*
 * GoalToLinkMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalToLinkMap.h"

namespace PredictivePowertrain {

GoalToLinkMap::GoalToLinkMap() {
	// todo GoalToLinkMap
}

GoalToLinkMap::GoalToLinkMap(GoalToLinkMap& other) {
	// todo GoalToLinkMap(GoalToLinkMap& other)
}

int GoalToLinkMap::linkTraversed(Link* link, Goal* goal) {
	int goalHash = goal->getHash();
	GoalMapEntry goalEntry;
	if(!this->goalMap.hashInMap(goalHash)) {
		goalEntry = GoalMapEntry(goal);
		this->goalMap.addEntry(goalHash, &goalEntry);
	} else {
		goalEntry = *this->goalMap.getEntry(goalHash);
	}
	goalEntry.incrementCount();
	int linkHash = link->getHash();
	int count = 1 + this->linkHashCounts.getEntry(linkHash);
	this->linkHashCounts.addEntry(linkHash, count);
	return count;
}

double** GoalToLinkMap::probabilityOfGoalsGivenLink(Link * link, Goal *goal, bool isSimilar) {
	// todo probabilityOfGoalGivenLink(Link * link, Goal *goal, bool isSimilar)
	this->goalMap.initializeCounter();
	GenericEntry<int, GoalMapEntry*> * goalEntry = this->goalMap.nextEntry();
	double** prob = new double*[this->goalMap.getSize()];
	int probCount = 0;
	int totalLinkCount = 0;
	while(goalEntry->key != -1) {
		prob[probCount] = new double[2];
		int linkCount;
		int linkHash = link->getHash();
		if(goal->isSimilar((goalEntry->value)->getGoalPtr())) {
			linkCount = this->linkHashCounts.getEntry(linkHash);
		} else {
			linkCount = 0;
		}
		int goalHash = (goalEntry->value)->getGoalPtr()->getHash();
		totalLinkCount += linkCount;
		prob[probCount][0]= goalHash;
		prob[probCount][1] = linkCount;
		probCount++;
		goalEntry = this->goalMap.nextEntry();
	}
	/*
	 * line 61 in matlab version -- I believe it is supposed to return the average of all the
	 * probabilities, so that is what I am doing here.
	 * CHECK OUT THE CHANGE BELOW
	*/
	int probLength = sizeof(*prob)/sizeof(int);
	for (int i = 0; i < probLength; i++) {
		prob[i][1] /= (double)((int)(totalLinkCount>0)*totalLinkCount+(int)(totalLinkCount<=0));
	}
	return prob;
}

double GoalToLinkMap::probabilityOfGoalGivenLink(Link* link, Goal* goal, bool isSimilar) {
	// TODO probabilityOfGoalGivenLink
}

GoalToLinkMap::~GoalToLinkMap() {
	// todo ~GoalToLinkMap()
}

} /* namespace PredictivePowertrain */
