/*
 * RoutePrediction.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "RoutePrediction.h"
#include "LinkToStateMap.h"
#include "GoalToLinkMap.h"
#include "../driver_prediction/Link.h"
#include "Goal.h"

using namespace DriverPrediction;
using namespace RoutePrediction;

namespace RoutePrediction {

RoutePredictionObj::RoutePredictionObj() {
	// TODO Auto-generated constructor stub
}

RoutePredictionObj::RoutePredictionObj(CityObj* city) {
	this->city = city;
}

Route* RoutePredictionObj::startPrediction(Intersection* currentIntersection, int currentCondition) {
}

Route* RoutePredictionObj::predict(Link* linkTaken) {
}

void RoutePredictionObj::updateStates(Link* linkTaken) {
}

Route* RoutePredictionObj::predictPrivate(Route* currentRoute) {
}

Route* RoutePredictionObj::createRoute() {
}

Route* RoutePredictionObj::createRouteConditions(int time, int day, int loc) {
}

Route* RoutePredictionObj::createRouteIntersection(Intersection* intersection,
		int time, int day, int loc) {
}

void RoutePredictionObj::parseRoute(Route* route) {
}

RoutePredictionObj::~RoutePredictionObj() {
	// TODO Auto-generated destructor stub
}

} /* namespace RoutePrediction */
