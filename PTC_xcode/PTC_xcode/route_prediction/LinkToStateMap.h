/*
 * LinkToStateMap.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_LINKTOSTATEMAP_H_
#define ROUTE_PREDICTION_LINKTOSTATEMAP_H_



#include "../map/GenericMap.h"
#include "../map/GenericEntry.h"

#include "../driver_prediction/Link.h"

#include "GoalMapEntry.h"
#include "Goal.h"
#include "LinkToStateMapEntry.h"
#include "Probability.h"

namespace PredictivePowertrain {

class LinkToStateMap {
private:
	GenericMap<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>* goalMap;

public:
    GenericMap<long int, LinkToStateMapEntry*>* getLinkMap();
    GenericMap<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>* getGoalMap();
	int incrementTransition(Link* lj, Goal* gj, Link* li);
	float getProbability(Link* li, Link* lj, Goal* gj, bool isSimilar);
	LinkToStateMap();
	LinkToStateMap(LinkToStateMap& other);
	virtual ~LinkToStateMap();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_LINKTOSTATEMAP_H_ */

