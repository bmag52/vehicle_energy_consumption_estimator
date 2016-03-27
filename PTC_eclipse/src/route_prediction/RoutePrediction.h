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
#include "GenericMap.h"

using namespace DriverPrediction;
using namespace City;
using namespace std;

namespace RoutePrediction {

class RoutePredictionObj {
private:
	LinkToStateMap linkToState;
	GoalToLinkMap goalToLink;
	GenericMap<int, Link*> links;
	GenericMap<int, Goal*> goals;
	GenericMap<int, pair<Link*,Goal*>*> states;
	Route* predictedRoute;
	Intersection* currentIntersection;
	double *probabilities;
	CityObj *city;
	Goal predictedGoal;
	double minInitialProbability;
	int probabilitySize;
	Route* unknownRoute;
	Link link;

	void updateStates(Link* link);
	void getNextState(int* hash, Goal* goal);
	Route* predictPrivate(Route* currentRoute);
	Route* createRoute();
	Route* createRouteConditions(int* currentCondition);
	Route* createRouteIntersection(Intersection* intersection, int* currentCondition);

public:
	RoutePredictionObj();
	RoutePredictionObj(CityObj* city);
	Route* startPrediction(Intersection* currentIntersection, int* currentCondition);
	Route* predict(Link* linkTaken);
	void parseRoute(Route* route);
	~RoutePredictionObj();
};


} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
