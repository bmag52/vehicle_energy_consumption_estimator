/*
 * GoalMapEntry.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalMapEntry.h"

namespace PredictivePowertrain {

GoalMapEntry::GoalMapEntry() {
	m = 0;
	maps = GenericMap();
}

GoalMapEntry::GoalMapEntry(Goal* goal) {
	m = 0;
	this->goal = goal;
	maps = GenericMap();
}

GoalMapEntry::~GoalMapEntry() {
}

void GoalMapEntry::incrementCount() {
	m++;
}

Goal* GoalMapEntry::getGoalPtr() {
	return &this->goal;
}

LinkToStateMapEntry* GoalMapEntry::getMapEntry(int key) {
	return this->maps.getEntry(key);
}

int GoalMapEntry::getM() {
	return this->m;
}

} /* namespace PredictivePowertrain */
