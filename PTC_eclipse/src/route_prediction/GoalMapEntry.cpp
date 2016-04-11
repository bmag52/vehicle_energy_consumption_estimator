/*
 * GoalMapEntry.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "GoalMapEntry.h"

namespace PredictivePowertrain {

GoalMapEntry::GoalMapEntry() {
}

GoalMapEntry::GoalMapEntry(Goal* goal) {
}

void GoalMapEntry::incrementCount() {
}

GoalMapEntry::~GoalMapEntry() {
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
