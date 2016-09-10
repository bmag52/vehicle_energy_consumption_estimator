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
    this->goalMap = new GenericMap<int, GoalMapEntry<int, int>*>();
}

GoalToLinkMap::GoalToLinkMap(GoalToLinkMap& other)
{
    this->goalMap = other.getGoalMap()->copy();
}

int GoalToLinkMap::linkTraversed(Link* link, Goal* goal)
{
	int goalHash = goal->getHash();
	GoalMapEntry<int, int>* goalEntry;
	if(!this->goalMap->hasEntry(goalHash)) {
		goalEntry = new GoalMapEntry<int, int>(goal);
		this->goalMap->addEntry(goalHash, goalEntry);
	} else {
		goalEntry = this->goalMap->getEntry(goalHash);
	}
	goalEntry->incrementCount();
    
	int linkHash = link->getHash();
	int count = 1 + goalEntry->getMapEntry(linkHash);
	goalEntry->addMapEntry(linkHash, count);
	return count;
}

double** GoalToLinkMap::probabilityOfGoalsGivenLink(Link* link, Goal* goal, bool isSimilar)
{
	this->goalMap->initializeCounter();
	GenericEntry<int, GoalMapEntry<int, int>*>* goalEntry = this->goalMap->nextEntry();
    
    int probLength = this->goalMap->getSize();
	double** prob = new double*[probLength];
    
	int probCount = 0;
	int totalLinkCount = 0;
    
	while(goalEntry != NULL) {
		prob[probCount] = new double[2];
		int linkCount;
        
		int linkHash = link->getHash();
		if(goal->isSimilar(goalEntry->value->getGoal())) {
            linkCount = goalEntry->value->getMapEntry(link->getHash());
		} else {
			linkCount = 0;
		}
		int goalHash = goalEntry->value->getGoal()->getHash();
        
		totalLinkCount += linkCount;
		prob[probCount][0]= goalHash;
		prob[probCount][1] = linkCount;
        
		goalEntry = this->goalMap->nextEntry();
        probCount++;
	}
    
	for (int i = 0; i < probLength; i++) {
        double prob_i = prob[i][1] / (double)( (totalLinkCount > 0 ) * totalLinkCount + (totalLinkCount <= 0));
        prob[i][1] = prob_i;
	}
	return prob;
}

double GoalToLinkMap::probabilityOfGoalGivenLink(Link * link, Goal * goal, bool isSimilar)
{
	double** matrix = this->probabilityOfGoalsGivenLink(link, goal, isSimilar);
	int matrixLength = sizeof(matrix)/sizeof(int);
	double goalHash = (double) goal->getHash();
	for(int i = 0; i < matrixLength; i++) {
		if(matrix[i][0] == goalHash) {
			return matrix[i][1];
		}
	}
	return 0;

}
    
GenericMap<int, GoalMapEntry<int, int>*>* GoalToLinkMap::getGoalMap()
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
}

} /* namespace PredictivePowertrain */
