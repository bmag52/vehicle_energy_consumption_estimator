/*
 * GoalMapEntry.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalMapEntry.h"

namespace PredictivePowertrain {

GoalMapEntry::GoalMapEntry() {
	this->m = 0;
}

GoalMapEntry::GoalMapEntry(Goal* goal) {
	this->m = 0;
	this->goal = goal;
}

GoalMapEntry::~GoalMapEntry() {
}

void GoalMapEntry::incrementCount() {
	this->m++;
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
