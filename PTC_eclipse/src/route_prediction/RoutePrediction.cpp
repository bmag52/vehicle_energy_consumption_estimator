/*
 * RoutePrediction.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "RoutePrediction.h"
#include "LinkToStateMap.h"
#include "GoalToLinkMap.h"
#include "GenericEntry.h"
#include "../driver_prediction/Link.h"
#include "Goal.h"

#include <algorithm>

using namespace DriverPrediction;
using namespace RoutePrediction;
using namespace std;

namespace RoutePrediction {

RoutePredictionObj::RoutePredictionObj() {
	// TODO Auto-generated constructor stub
}

RoutePredictionObj::RoutePredictionObj(CityObj* city) {
	this->city = city;
	Link unknownLink(-1, -1);
	Goal unknownGoal(-1);
	Route unknownRoute(&unknownLink, &unknownGoal);
	this->unknownRoute = &unknownRoute;
}

Route* RoutePredictionObj::startPrediction(Intersection* currentIntersection, int* currentCondition) {
	this->predictedGoal = Goal(1, currentCondition);

	Link* nextLinks;
	currentIntersection->getOutgoingLinks(nextLinks);

	this->probabilitySize = sizeof(*nextLinks)/sizeof(Link)*this->goals.getSize();
	this->probabilities = new double[this->probabilitySize];

	this->goals.initializeCounter();
	GenericEntry<int, Goal*> nextEntry = this->goals.nextEntry();
	int hash = nextEntry.key;
	Goal* goal = nextEntry.value;

	// creating the probability of each goal based on its relation to the conditions
	int counter = 1;
	while(hash != -1)
	{
		for(int i = 0; i < sizeof(*nextLinks)/sizeof(Link); i++)
		{
			double goalProbability = this->goalToLink.probabilityOfGoalGivenLink(&nextLinks[i],goal,0);
			if(this->predictedGoal.isSimilar(goal))
			{
				// high probability since condition is right
				goalProbability *= (*goal).getNumSeen();
			} else {
				// lower probability since condition is wrong
				goalProbability *= .1*(*goal).getNumSeen();
			}

			pair<Link*, Goal*> thisPair (&nextLinks[i], goal);
			this->states.addEntry(counter, &thisPair);

			this->probabilities[counter] = max(this->minInitialProbability*1/this->goals.getSize(),goalProbability);
			counter++;
		}
		GenericEntry<int, Goal*> nextEntry = this->goals.nextEntry();
		hash = nextEntry.key;
		goal = nextEntry.value;
	}

	// normalize probabilites
	int sum = 0;
	for(int i = 0; i < this->probabilitySize; i++) { sum += this->probabilities[i]; }
	for(int i = 0; i < this->probabilitySize; i++) { this->probabilities[i] /= sum; }

	this->predictedRoute = predictPrivate(NULL);
	this->currentIntersection = currentIntersection;
	return createRouteConditions(currentCondition);
}

Route* RoutePredictionObj::predict(Link* linkTaken) {
}

void RoutePredictionObj::updateStates(Link* linkTaken) {
}

Route* RoutePredictionObj::predictPrivate(Route* currentRoute) {

	// check for uninitialized probability values
	for(int i = 0; i < this->probabilitySize; i++)
	{
		if(this->probabilities[i] == 0) { return this->unknownRoute; }
	}

	// get most probable state
	int maxProbability = -1;
	int nextStateIndex = -1;
	for(int i = 0; i < this->probabilitySize; i++)
	{
		if(this->probabilities[i] > maxProbability)
		{
			maxProbability = this->probabilities[i];
			nextStateIndex = i;
		}
	}

	// get state with highest probability and add it to route
	pair<Link*, Goal*> *nextState = this->states.getEntry(nextStateIndex);
	Route* route;
	if(currentRoute != NULL)
	{
		route = currentRoute;
	}
	(*route).addlink((*nextState).first);
	if((*(*nextState).first).isEqual(this->link.finalLink()))
	{

	}
}

Route* RoutePredictionObj::createRoute() {
}

Route* RoutePredictionObj::createRouteConditions(int* currentConditions) {
}

Route* RoutePredictionObj::createRouteIntersection(Intersection* intersection, int* currentConditions) {
}

void RoutePredictionObj::parseRoute(Route* route) {
}

RoutePredictionObj::~RoutePredictionObj() {
	// TODO Auto-generated destructor stub
}

} /* namespace RoutePrediction */
