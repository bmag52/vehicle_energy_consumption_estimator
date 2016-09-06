/*
 * GoalMapEntry.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalMapEntry.h"

namespace PredictivePowertrain {

GoalMapEntry::GoalMapEntry()
{
    this->goal = new Goal();
    this->initialize();
}

GoalMapEntry::GoalMapEntry(Goal* goal)
{
	this->goal = goal;
    this->initialize();
}
    
void GoalMapEntry::initialize()
{
    this->m = 0;
    this->map = new GenericMap<int, LinkToStateMapEntry*>();
}

GoalMapEntry::~GoalMapEntry()
{
    free(this->goal);
    free(this->map);
}

void GoalMapEntry::addMapEntry(int key, LinkToStateMapEntry* value)
{
    this->map->addEntry(key, value);
}

void GoalMapEntry::incrementCount()
{
	this->m++;
}

Goal* GoalMapEntry::getGoal()
{
	return this->goal;
}

LinkToStateMapEntry* GoalMapEntry::getMapEntry(int key)
{
	return this->map->getEntry(key);
}

int GoalMapEntry::getM()
{
	return this->m;
}
    
GenericMap<int, LinkToStateMapEntry*>* GoalMapEntry::getMap()
{
    return this->map;
}

} /* namespace PredictivePowertrain */
