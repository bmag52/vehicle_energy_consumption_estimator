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
    int count = 1;
    if(goalEntry->getMap()->hasEntry(linkHash))
    {
        count += goalEntry->getMapEntry(linkHash);
        goalEntry->getMap()->updateEntry(linkHash, count);
    }
    else
    {
        goalEntry->addMapEntry(linkHash, count);
    }
	return count;
}

std::vector<std::vector<float>*>* GoalToLinkMap::probabilityOfGoalsGivenLink(Link* link, Goal* goal, bool isSimilar)
{
	this->goalMap->initializeCounter();
	GenericEntry<int, GoalMapEntry<int, int>*>* goalEntry = this->goalMap->nextEntry();
    
    int probLength = this->goalMap->getSize();
    std::vector<std::vector<float>*>* prob = new std::vector<std::vector<float>*>(probLength);
    
	int probCount = 0;
	int totalLinkCount = 0;
    
	while(goalEntry != NULL) {
        prob->at(probCount) = new std::vector<float>(2);
		int linkCount = 0;
        
		if(goal->isSimilar(goalEntry->value->getGoal()) && goalEntry->value->getMap()->hasEntry(link->getHash()))
        {
            linkCount = goalEntry->value->getMapEntry(link->getHash());
            totalLinkCount += linkCount;
		}
		int goalHash = goalEntry->value->getGoal()->getHash();

		prob->at(probCount)->at(0) = goalHash;
		prob->at(probCount)->at(1) = linkCount;
        
		goalEntry = this->goalMap->nextEntry();
        probCount++;
	}
    
	for (int i = 0; i < probLength; i++) {
        float prob_i = prob->at(i)->at(1) / ((totalLinkCount > 0 ) * totalLinkCount + (totalLinkCount <= 0));
        prob->at(i)->at(1) = prob_i;
	}
	return prob;
}
    
GenericMap<float, float>* GoalToLinkMap::probabilityOfGoalsGivenLinkMap(Link * link, Goal * goal, bool isSimilar)
{
    std::vector<std::vector<float>*>* matrix = this->probabilityOfGoalsGivenLink(link, goal, isSimilar);
    GenericMap<float, float>* result = new GenericMap<float, float>();
    size_t matrixLength = matrix->size();
    for(int i = 0; i < matrixLength; i++)
    {
        result->addEntry(matrix->at(i)->at(0), matrix->at(i)->at(1));
    }
    return result;
}

float GoalToLinkMap::probabilityOfGoalGivenLink(Link * link, Goal * goal, bool isSimilar)
{
    std::vector<std::vector<float>*>* matrix = this->probabilityOfGoalsGivenLink(link, goal, isSimilar);
	size_t matrixLength = matrix->size();
	double goalHash = (double) goal->getHash();
	for(int i = 0; i < matrixLength; i++) {
		if(matrix->at(i)->at(0) == goalHash) {
            float probability = matrix->at(i)->at(1);
            delete(matrix);
			return probability;
		}
	}
	return 0;
}
    
GenericMap<int, GoalMapEntry<int, int>*>* GoalToLinkMap::getGoalMap()
{
    return this->goalMap;
}

GoalToLinkMap::~GoalToLinkMap()
{
    delete(this->goalMap);
}

} /* namespace PredictivePowertrain */
