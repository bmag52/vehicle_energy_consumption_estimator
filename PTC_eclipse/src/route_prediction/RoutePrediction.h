/*
 * RoutePrediction.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_ROUTEPREDICTION_H_
#define ROUTE_PREDICTION_ROUTEPREDICTION_H_

#include "LinkToStateMap.h"
#include "GoalToLinkMap.h"
#include "../driver_prediction/Link.h"
#include "Goal.h"


namespace RoutePrediction {

class RoutePrediction {
private:
	LinkToStateMap linkToState;
	GoalToLinkMap goalToState;
	GenericMap<int, int> links;
	GenericMap<int, int> goals;
	GenericMap<int, int> states;



public:
	RoutePrediction();
	~RoutePrediction();
};


} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
