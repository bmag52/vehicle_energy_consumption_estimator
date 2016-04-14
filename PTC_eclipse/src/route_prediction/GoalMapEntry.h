/*
 * GoalMapEntry.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALMAPENTRY_H_
#define ROUTE_PREDICTION_GOALMAPENTRY_H_
#include "../map/GenericMap.h"
#include "Goal.h"

namespace PredictivePowertrain {


class GoalMapEntry {
public:
	GenericMap<int,int> maps;
	Goal goal;
	int m;
	GoalMapEntry();
	GoalMapEntry(GoalMapEntry& other);
	void increment_count();
	virtual ~GoalMapEntry();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GOALMAPENTRY_H_ */
