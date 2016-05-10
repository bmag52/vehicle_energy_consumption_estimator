/*
 * GoalMapEntry.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALMAPENTRY_H_
#define ROUTE_PREDICTION_GOALMAPENTRY_H_
#include "../map/GenericMap.h"
#include "LinkToStateMapEntry.h"
#include "Goal.h"

namespace PredictivePowertrain {

class GoalMapEntry {
private:
	GenericMap<int, LinkToStateMapEntry*> maps;
	Goal goal;
	int m;

public:
	GoalMapEntry();
	GoalMapEntry(Goal* goal);
	void incrementCount();
	virtual ~GoalMapEntry();
	Goal* getGoalPtr();
	LinkToStateMapEntry* getMapEntry(int key);
	int getM();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GOALMAPENTRY_H_ */
