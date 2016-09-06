/*
 * GoalToLinkMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalToLinkMap.h"

namespace PredictivePowertrain {

GoalToLinkMap::GoalToLinkMap()
{
    this->linkHashCounts = new GenericMap<int, int>();
    this->goalMap = new GenericMap<int, GoalMapEntry*>();
}

GoalToLinkMap::GoalToLinkMap(GoalToLinkMap& other)
{
    this->goalMap = other.getGoalMap()->copy();
    this->linkHashCounts = other.getLinkHashCounts()->copy();
}

int GoalToLinkMap::linkTraversed(Link* link, Goal* goal)
{
	int goalHash = goal->getHash();
	GoalMapEntry* goalEntry;
	if(!this->goalMap->hasEntry(goalHash)) {
		goalEntry = new GoalMapEntry(goal);
		this->goalMap->addEntry(goalHash, goalEntry);
	} else {
		goalEntry = this->goalMap->getEntry(goalHash);
	}
	goalEntry->incrementCount();
	int linkHash = link->getHash();
	int count = 1 + this->linkHashCounts->getEntry(linkHash);
	this->linkHashCounts->addEntry(linkHash, count);
	return count;
}

double** GoalToLinkMap::probabilityOfGoalsGivenLink(Link* link, Goal* goal, bool isSimilar)
{
	this->goalMap->initializeCounter();
	GenericEntry<int, GoalMapEntry*> * goalEntry = this->goalMap->nextEntry();
	double** prob = new double*[this->goalMap->getSize()];
	int probCount = 0;
	int totalLinkCount = 0;
	while(goalEntry->key != -1) {
		prob[probCount] = new double[2];
		int linkCount;
		int linkHash = link->getHash();
		if(goal->isSimilar((goalEntry->value)->getGoal())) {
			linkCount = this->linkHashCounts->getEntry(linkHash);
		} else {
			linkCount = 0;
		}
		int goalHash = (goalEntry->value)->getGoal()->getHash();
		totalLinkCount += linkCount;
		prob[probCount][0]= goalHash;
		prob[probCount][1] = linkCount;
		probCount++;
		goalEntry = this->goalMap->nextEntry();
	}
	int probLength = sizeof(*prob)/sizeof(int);
	for (int i = 0; i < probLength; i++) {
		prob[i][1] /= (double)((int)(totalLinkCount>0)*totalLinkCount+(int)(totalLinkCount<=0));
	}
	return prob;
}

double GoalToLinkMap::probabilityOfGoalGivenLink(Link * link, Goal * goal, bool isSimilar)
{
	double** matrix = this->probabilityOfGoalsGivenLink(link, goal, isSimilar);
	int matrixLength = sizeof(*matrix)/sizeof(int);
	double goalHash = (double) goal->getHash();
	for(int i = 0; i < matrixLength; i++) {
		if(matrix[i][0] == goalHash) {
			return matrix[i][1];
		}
	}
	return 0;

}

GenericMap<int, int>* GoalToLinkMap::getLinkHashCounts()
{
    return this->linkHashCounts;
}
    
GenericMap<int, GoalMapEntry*>* GoalToLinkMap::getGoalMap()
{
    return this->goalMap;
}
    
GenericMap<int, int>* GoalToLinkMap::probabilityOfGoalsGivenLinkMap(Link * link, Goal * goal, bool isSimilar)
{
	double** matrix = this->probabilityOfGoalsGivenLink(link, goal, isSimilar);
	GenericMap<int, int>* result = new GenericMap<int, int>();
	int matrixLength = sizeof(*matrix) / sizeof(int);
	for(int i = 0; i < matrixLength; i++)
    {
		result->addEntry(matrix[i][0], matrix[i][1]);
	}
	return result;
}

GoalToLinkMap::~GoalToLinkMap()
{
    free(this->goalMap);
    free(this->linkHashCounts);
}

} /* namespace PredictivePowertrain */
