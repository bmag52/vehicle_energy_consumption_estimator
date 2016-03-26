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
#include "../city/City.h"
#include "Goal.h"
#include "stdlib.h"

#include <algorithm>

using namespace DriverPrediction;
using namespace City;
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

void RoutePredictionObj::getNextState(int * hash, Goal * goal) {
	GenericEntry<int, Goal*>* nextEntry = this->goals.nextEntry();
	hash = &nextEntry->key;
	goal = nextEntry->value;
}

Route* RoutePredictionObj::startPrediction(Intersection* currentIntersection, int* currentCondition) {
	this->predictedGoal = Goal(1, currentCondition);

	Link* nextLinks;
	currentIntersection->getOutgoingLinks(nextLinks);

	this->probabilitySize = sizeof(*nextLinks)/sizeof(Link)*this->goals.getSize();
	this->probabilities = new double[this->probabilitySize];

	int* hash;
	Goal* goal;
	this->goals.initializeCounter();
	getNextState(hash, goal);

	// creating the probability of each goal based on its relation to the conditions
	int counter = 1;
	while((*hash) != -1)
	{
		for(int i = 0; i < sizeof(*nextLinks)/sizeof(Link); i++)
		{
			double goalProbability = this->goalToLink.probabilityOfGoalGivenLink(&nextLinks[i],goal,false);
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
		getNextState(hash, goal);
	}

	// normalize probabilites
	int sum = 0;
	for(int i = 0; i < this->probabilitySize; i++) { sum += this->probabilities[i]; }
	for(int i = 0; i < this->probabilitySize; i++) { this->probabilities[i] /= sum; }

	this->currentIntersection = currentIntersection;
	this->predictedRoute = predictPrivate(NULL);
	return createRouteConditions(currentCondition);
}

Route* RoutePredictionObj::predict(Link* linkTaken) {
}

void RoutePredictionObj::updateStates(Link* chosenLink) {

	// get next links
	Link* nextLinks = this->city->getNextLinks(chosenLink);

	// generate new data structures
	GenericMap<int, pair<Link*,Goal*>*> newStates;
	int newProbabilitySize = sizeof(*nextLinks)/sizeof(Link)*this->goals.getSize();
	double* newProbabilities = new double[newProbabilitySize];

	// generate reused fields
	int* hash;
	Link* li;
	Goal* gi, gj;
	double pSi, pGl, pLs, minProbability;
	pair<Link*, Goal*>* sj;
	int counter = 0;

	// calculate new states and probabilities
	for(int i = 0; i < sizeof(*nextLinks)/sizeof(Link); i++)
	{
		this->goals.initializeCounter();
		getNextState(hash, gi);
		li = &nextLinks[i];

		while((*hash) != -1)
		{
			pSi = 0;
			for(int j = 0; j < this->states.getSize(); j++)
			{
				sj = this->states.getEntry(j);
				if(sj->first->isEqual(li))
				{
					gj = sj->second;
					minProbability = this->minInitialProbability / this->goals.getSize();
					pGl = this->goalToLink.probabilityOfGoalGivenLink(li, gi, 0);
					pLs = this->linkToState.getProbability(li, chosenLink, gj, false);
					pSi += max(minProbability,this->probabilities[j])*max(minProbability,pLs)*max(minProbability,pGl);
				}
			}
			pair<Link*, Goal*> si (&nextLinks[i], gi);
			newStates.addEntry(counter, &si);
			newProbabilities[counter] = pSi;
			getNextState(hash, gi);
		}
	}

	// normalize probabilities
	int sum = 0;
	for(int i = 0; i < newProbabilitySize; i++) { sum += newProbabilities[i]; }
	for(int i = 0; i < newProbabilitySize; i++) { newProbabilities[i] /= sum; }

	// update probabilities and states
	free(this->probabilities);
	free(&this->probabilitySize);
	free(&this->states);

	this->probabilities = newProbabilities;
	this->probabilitySize = newProbabilitySize;
	this->states = newStates;
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
	(*currentRoute).addlink(nextState->first);

	if(!(*nextState->first).isEqual(this->link.finalLink()))
	{
		updateStates(nextState->first);
		return predictPrivate(currentRoute);
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
