/*
 * GoalMapEntry.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALMAPENTRY_H_
#define ROUTE_PREDICTION_GOALMAPENTRY_H_
#include "GenericMap.h"
#include "Goal.h"

namespace RoutePrediction {


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

} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_GOALMAPENTRY_H_ */
