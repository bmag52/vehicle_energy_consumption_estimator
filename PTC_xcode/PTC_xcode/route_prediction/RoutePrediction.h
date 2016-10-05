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
#include <vector>

using namespace std;

namespace PredictivePowertrain {

class RoutePrediction {
private:
	LinkToStateMap* linkToState;
	GoalToLinkMap* goalToLink;
	GenericMap<int, Link*>* links;
	GenericMap<int, Goal*>* goals;
    GenericMap<int, std::pair<Link*,Goal*>*>* states;
	Route* predictedRoute;
	Route* currentRoute;
    std::vector<float>* probabilities;
	City *city;
	Goal *predictedGoal;
	float minInitialProbability;
	Route* unknownRoute;
	Route* overRoute;
	Link link;

	std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>* updateStates(Link* chosenLink, GenericMap<int, std::pair<Link*,Goal*>*>* oldStates, std::vector<float>* oldProbabilites);
	Route* predictPrivate(Route* currentRoute, GenericMap<int, std::pair<Link*,Goal*>*>* currentStates, std::vector<float>* currentProbabilities);
	Route* createRoute();
    Route* createRouteConditions(std::vector<float>* currentCondition);
    Route* createRouteIntersection(Intersection* intersection, std::vector<float>* currentCondition);
    void initialize();

public:
	RoutePrediction();
	RoutePrediction(City* city);
    virtual ~RoutePrediction();
    Route* startPrediction(Intersection* currentIntersection, std::vector<float>* currentCondition);
	Route* predict(Link* linkTaken);
	void parseRoute(Route* route);
};


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
