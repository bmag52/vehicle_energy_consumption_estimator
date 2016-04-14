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
	if(!goalMap.hashInMap(goalHash)) {
		goalEntry = GoalMapEntry(goal);
		goalMap.addEntry(goalHash, &goalEntry);
	} else {
		goalEntry = goalMap.getEntry(goalHash);
	}
	goalEntry.incrementCount();
	int linkHash = link->getHash();
	int count = 1 + linkHashCounts.getEntry(linkHash);
	linkMap.addEntry(linkHash, count);
	return count;
}

double GoalToLinkMap::probabilityOfGoalGivenLink(Link * link, Goal *goal, bool isSimilar) {
	// todo probabilityOfGoalGivenLink(Link * link, Goal *goal, bool isSimilar)
	goalMap.initializeCounter();
	GenericEntry<int, GoalMapEntry*> * goalEntry = goalMap.nextEntry();
	int prob[goalMap.getSize()][2];
	int probCount = 0;
	int totalLinkCount = 0;
	while(goalEntry->key != -1) {
		int linkCount;
		int linkHash = link->getHash();
		if(goal->isSimilar((goalEntry->value)->goal)) {
			linkCount = linkHashCounts.getEntry(linkHash);
		} else {
			linkCount = 0;
		}
		int goalHash = (goalEntry->value)->goal.getHash();
		totalLinkCount = totalLinkCount + linkCount;
		prob[probCount][0]= goalHash;
		prob[probCount][1] = linkCount;
		probCount++;
		goalEntry = goalMap.nextEntry();
	}
	/*
	 * line 61 in matlab version -- I believe it is supposed to return the average of all the
	 * probabilities, so that is what I am doing here.
	*/
	double probSum = 0.0;
	for (int i = 0; i < prob.length; i++) {
		probSum += prob[i][1];
	}
	return probSum / prob.length;
}

GoalToLinkMap::~GoalToLinkMap() {
	// todo ~GoalToLinkMap()
}

} /* namespace PredictivePowertrain */
