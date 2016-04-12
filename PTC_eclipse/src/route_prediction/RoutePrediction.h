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
#include "../map/GenericEntry.h"
#include "Route.h"
#include "../driver_prediction/Link.h"
#include "../city/City.h"
#include "Goal.h"
#include "../city/Intersection.h"
#include "../map/GenericMap.h"

#include <algorithm>
#include <assert.h>
#include "stdlib.h"

using namespace std;

namespace PredictivePowertrain {

class RoutePrediction {
private:
	LinkToStateMap linkToState;
	GoalToLinkMap goalToLink;
	GenericMap<int, Link*> links;
	GenericMap<int, Goal*> goals;
	GenericMap<int, pair<Link*,Goal*>*> states;
	Route* predictedRoute;
	Route* currentRoute;
	double *probabilities;
	City *city;
	Goal predictedGoal;
	double minInitialProbability;
	int probabilitySize;
	Route* unknownRoute;
	Route* overRoute;
	Link link;

	void updateStates(Link* chosenLink, GenericMap<int, pair<Link*,Goal*>*>* oldStates, double* oldProbabilities);
	void getNextState(int* hash, Goal* goal);
	double* copyProbs();
	Route* predictPrivate(Route* currentRoute, GenericMap<int, pair<Link*,Goal*>*>* currentStates, double* currentProbabilities);
	Route* createRoute();
	Route* createRouteConditions(int* currentCondition);
	Route* createRouteIntersection(Intersection* intersection, int* currentCondition);

public:
	RoutePrediction();
	RoutePrediction(City* city);
	Route* startPrediction(Intersection* currentIntersection, int* currentCondition);
	Route* predict(Link* linkTaken);
	void parseRoute(Route* route);
	~RoutePrediction();
};


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
