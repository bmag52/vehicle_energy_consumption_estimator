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
#include <assert.h>

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
	Link overLink(1,1);
	Goal unknownGoal(-1);
	Goal overGoal(1);
	Route unknownRoute(&unknownLink, &unknownGoal);
	Route overRoute(&overLink, &overGoal);
	this->unknownRoute = &unknownRoute;
	this->overRoute = &overRoute;
}

void RoutePredictionObj::getNextState(int * hash, Goal * goal) {
	GenericEntry<int, Goal*>* nextEntry = this->goals.nextEntry();
	hash = &nextEntry->key;
	goal = nextEntry->value;
}

Route* RoutePredictionObj::startPrediction(Intersection* currentIntersection, int* currentCondition) {
	this->predictedGoal = Goal(1, currentCondition);

	Link* nextLinks = currentIntersection->getOutgoingLinks();

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

	// normalize probabilities
	int sum = 0;
	for(int i = 0; i < this->probabilitySize; i++) { sum += this->probabilities[i]; }
	for(int i = 0; i < this->probabilitySize; i++) { this->probabilities[i] /= sum; }

	this->currentRoute->setToIntersection(currentIntersection);
	this->predictedRoute = predictPrivate(NULL, this->states.copy(), copyProbs());
	return createRouteConditions(currentCondition);
}

double* RoutePredictionObj::copyProbs() {
	double* copy = new double[this->probabilitySize];
	for(int i = 0; i < this->probabilitySize; i++)
	{
		copy[i] = this->probabilities[i];
	}
	return copy;
}

Route* RoutePredictionObj::predict(Link* linkTaken) {
	Link* legalLinks;
	if(this->currentRoute->isIntersection())
	{
		legalLinks = this->currentRoute->getIntersectionPtr()->getOutgoingLinks();
		free(this->currentRoute);
	} else {
		assert(linkTaken->isEqual(this->link.finalLink()) == false);
		legalLinks = this->city->getNextLinks(this->currentRoute->getLastLinkPtr());
	}

	// make sure that the link given is legal
	bool error = true;
	for(int i = 0; i < sizeof(*legalLinks)/sizeof(Link); i++)
	{
		if(legalLinks[i].isEqual(linkTaken))
		{
			error = false;
			break;
		}
	}

	// there is an error and the current route is known (ie, wasnt an error before)
    // if not not legal, stop and return an error value
	if(error && this->predictedRoute->isEmpty())
	{
		this->predictedRoute = this->unknownRoute;
		return this->unknownRoute;
	} else if (linkTaken->isEqual(this->link.finalLink())) {
		this->predictedRoute = this->overRoute;
		return this->overRoute;
	}

	updateStates(linkTaken, &this->states, this->probabilities);
	this->currentRoute->addlink(linkTaken);

	if(linkTaken->isEqual(this->predictedRoute->getEntry(0)))
	{
		this->predictedRoute->removeFirstLink();
	} else {
		this->predictedRoute = predictPrivate(NULL, this->states.copy(), copyProbs());
	}

	if(this->predictedRoute->isEmpty())
	{
		return this->predictedRoute;
	} else if (this->predictedRoute->getLinkSize() == 1) {
		return createRouteIntersection(this->city->getIntersectionFromLink(linkTaken), this->predictedGoal.getBins());
	} else {
		return createRoute();
	}
}

void RoutePredictionObj::updateStates(Link* chosenLink, GenericMap<int, pair<Link*,Goal*>*>* currentStates, double* currentProbabilities) {
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
			for(int j = 0; j < currentStates->getSize(); j++)
			{
				sj = currentStates->getEntry(j);
				if(sj->first->isEqual(li))
				{
					gj = sj->second;
					minProbability = this->minInitialProbability / this->goals.getSize();
					pGl = this->goalToLink.probabilityOfGoalGivenLink(li, gi, 0);
					pLs = this->linkToState.getProbability(li, chosenLink, gj, false);
					pSi += max(minProbability,currentProbabilities[j])*max(minProbability,pLs)*max(minProbability,pGl);
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
	free(currentProbabilities); free(currentStates);
	currentProbabilities = newProbabilities;
	currentStates = &newStates;
}

Route* RoutePredictionObj::predictPrivate(Route* currentRoute, GenericMap<int, pair<Link*,Goal*>*>* currentStates, double* currentProbabilities) {
	// check for uninitialized probability values
	int currentProbabilitySize = sizeof(*currentProbabilities)/sizeof(double);
	for(int i = 0; i < currentProbabilitySize; i++)
	{
		if(currentProbabilities[i] == 0) { return this->unknownRoute; }
	}

	// get most probable state
	int maxProbability = -1;
	int nextStateIndex = -1;
	for(int i = 0; i < currentProbabilitySize; i++)
	{
		if(currentProbabilities[i] > maxProbability)
		{
			maxProbability = currentProbabilities[i];
			nextStateIndex = i;
		}
	}

	// get state with highest probability and add it to route
	pair<Link*, Goal*> *nextState = currentStates->getEntry(nextStateIndex);
	(*currentRoute).addlink(nextState->first);

	if(!(*nextState->first).isEqual(this->link.finalLink()))
	{
		updateStates(nextState->first, currentStates, currentProbabilities);
		return predictPrivate(currentRoute, currentStates, currentProbabilities);
	}
	free(currentStates); free(currentProbabilities);
	return currentRoute;
}

Route* RoutePredictionObj::createRoute() {
	return createRouteConditions(this->predictedGoal.getBins());
}

Route* RoutePredictionObj::createRouteConditions(int* currentConditions) {
	int lastLinkIndex = this->predictedRoute->getLinkSize() - 1;
	return createRouteIntersection(this->city->getIntersectionFromLink(this->predictedRoute->getEntry(lastLinkIndex)), currentConditions);
}

Route* RoutePredictionObj::createRouteIntersection(Intersection* intersection, int* currentConditions) {
	free(&this->predictedGoal);
	this->predictedGoal = Goal(intersection->number, currentConditions);
	Route route(this->predictedRoute->getLinksPtr(), &this->predictedGoal);
	return &route;
}

void RoutePredictionObj::parseRoute(Route* route) {
	// get hash of route goal and add it to goals if nonexistent
	int goalHash = route->getGoalHash();
	if(this->goals.hashInMap(goalHash))
	{
		this->goals.getEntry(goalHash)->incrementNumSeen();
	} else {
		route->getGoalPtr()->setNumSeen(1);
		this->goals.addEntry(goalHash, route->getGoalPtr());
	}

	// add links to link map
	for(int i = 0; i < route->getLinkSize(); i++)
	{
		Link* li; Link* lj;
		lj = &route->getLinksPtr()[i];

		if(i == route->getLinkSize())
		{
			li = this->link.finalLink();
		} else {
			li = &route->getLinksPtr()[i+1];
		}

		this->linkToState.incrementTransition(lj, route->getGoalPtr(), li);
		this->goalToLink.linkTraversed(lj, route->getGoalPtr());
		this->links.addEntry(lj->getHash(), lj);
	}
}

RoutePredictionObj::~RoutePredictionObj() {
	// TODO Auto-generated destructor stub
}

} /* namespace RoutePrediction */
