/*
 * LinkToStateMap.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_LINKTOSTATEMAP_H_
#define ROUTE_PREDICTION_LINKTOSTATEMAP_H_

#include "../driver_prediction/Probability.h"

#include "../map/GenericMap.h"
#include "../map/GenericEntry.h"

#include "GoalMapEntry.h"
#include "Goal.h"
#include "LinkToStateMapEntry.h"
#include "../driver_prediction/Link.h"

namespace PredictivePowertrain {

class LinkToStateMap {
private:
	GenericMap<int, LinkToStateMapEntry*>* linkMap;
	GenericMap<int, GoalMapEntry<int, LinkToStateMapEntry*>*>* goalMap;

public:
    GenericMap<int, LinkToStateMapEntry*>* getLinkMap();
    GenericMap<int, GoalMapEntry<int, LinkToStateMapEntry*>*>* getGoalMap();
	int incrementTransition(Link* lj, Goal* gj, Link* li);
	float getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar);
	LinkToStateMap();
	LinkToStateMap(LinkToStateMap& other);
	virtual ~LinkToStateMap();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_LINKTOSTATEMAP_H_ */

