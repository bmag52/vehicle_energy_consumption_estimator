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
#include "Route.h"
#include "../driver_prediction/Link.h"
#include "../city/City.h"
#include "Goal.h"
#include "../city/Intersection.h"

using namespace DriverPrediction;
using namespace City;

namespace RoutePrediction {

class RoutePredictionObj {
private:
	LinkToStateMap linkToState;
	GoalToLinkMap goalToState;
	GenericMap<int, int> links;
	GenericMap<int, int> goals;
	GenericMap<int, int> states;
	Route predictedRoute;
	Route currentRoute;
	double *probabilities;
	CityObj *city;
	Goal predictedGoal;
	int minInitialProbability;

	void updateStates(Link* linkTaken);
	Route* predictPrivate(Route* currentRoute);
	Route* createRoute();
	Route* createRouteConditions(int time, int day, int loc);
	Route* createRouteIntersection(Intersection* intersection, int time, int day, int loc);

public:
	RoutePredictionObj();
	RoutePredictionObj(CityObj * city);
	Route* startPrediction(Intersection * currentIntersection, int currentCondition);
	Route* predict(Link * linkTaken);
	void parseRoute(Route* route);
	~RoutePredictionObj();
};


} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
