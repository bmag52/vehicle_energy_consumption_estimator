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

using namespace RoutePrediction;

class RoutePrediction {
private:
//	LinkToStateMap link_to_state;


public:
	RoutePrediction();
	~RoutePrediction();
};


} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
