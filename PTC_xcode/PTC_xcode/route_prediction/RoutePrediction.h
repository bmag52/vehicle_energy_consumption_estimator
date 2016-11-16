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
#include "Goal.h"
#include "Route.h"
#include "Probability.h"

#include "../map/GenericMap.h"
#include "../map/GenericEntry.h"

#include "../driver_prediction/Link.h"

#include "../city/City.h"
#include "../city/Intersection.h"

#include <algorithm>
#include <assert.h>
#include "stdlib.h"
#include <vector>

namespace PredictivePowertrain {

class RoutePrediction {
private:
	LinkToStateMap* linkToState;
	GoalToLinkMap* goalToLink;
	GenericMap<long int, Link*>* links;
	GenericMap<long int, Goal*>* goals;
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
    GenericMap<long int, Link*>* getLinks();
    GenericMap<long int, Goal*>* getGoals();
    LinkToStateMap* getLinkToState();
    GoalToLinkMap* getGoalToLink();
    void addPredictionElements(GenericMap<long int, Link*>* newLinks,
                               GenericMap<long int, Goal*>* newGoals,
                               GoalToLinkMap* newGoalToLink,
                               LinkToStateMap* newLinkToState);
    void addCity(City* newCity);
    Route* getPredictedRoute();
    City* getCity();
    Route* getUnknownRoute();
    Route* getOverRoute();
};


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_ROUTEPREDICTION_H_ */
