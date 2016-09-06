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
	GenericMap<int, LinkToStateMapEntry*>* map;
	Goal* goal;
	int m;
    void initialize();

public:
	GoalMapEntry();
	GoalMapEntry(Goal* goal);
	void incrementCount();
	virtual ~GoalMapEntry();
	Goal* getGoal();
	LinkToStateMapEntry* getMapEntry(int key);
    void addMapEntry(int key, LinkToStateMapEntry* val);
	int getM();
    GenericMap<int, LinkToStateMapEntry*>* getMap();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GOALMAPENTRY_H_ */
