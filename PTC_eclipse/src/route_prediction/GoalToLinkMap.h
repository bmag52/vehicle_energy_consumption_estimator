/*
 * GoalToLinkMap.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALTOLINKMAP_H_
#define ROUTE_PREDICTION_GOALTOLINKMAP_H_
#include "GenericMap.h"
#include "GoalMapEntry.h"
#include "../driver_prediction/Link.h"
#include "Goal.h"

using namespace DriverPrediction;

namespace RoutePrediction {

class GoalToLinkMap {
public:
	GenericMap<int,GoalMapEntry*> maps;
	int maps_size;
	GoalToLinkMap();
	GoalToLinkMap(GoalToLinkMap &other);
	int link_traversed(Link, Goal);
	double probability_of_goals_given_link(Link, Goal, bool);
	virtual ~GoalToLinkMap();
};

} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_GOALTOLINKMAP_H_ */
