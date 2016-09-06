/*
 * RoutePrediction.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "RoutePrediction.h"

namespace PredictivePowertrain {

RoutePrediction::RoutePrediction()
{
    initialize();
}

RoutePrediction::RoutePrediction(City* city)
{
	this->city = city;
    initialize();
}
    
void RoutePrediction::initialize()
{
    Link unknownLink(-1, -1);
    Link overLink(1,1);
    Goal unknownGoal(-1);
    Goal overGoal(1);
    
    this->unknownRoute = new Route(new GenericMap<int, Link*>(), &unknownGoal);
    this->overRoute = new Route(new GenericMap<int, Link*>(), &overGoal);;
    
    this->linkToState = new LinkToStateMap();
    this->goalToLink = new GoalToLinkMap();
    this->links = new GenericMap<int, Link*>();
    this->goals = new GenericMap<int, Goal*>();
    this->states = new GenericMap<int, std::pair<Link*, Goal*>*>();
    
}
    
RoutePrediction::~RoutePrediction()
{
    free(this->unknownRoute);
    free(this->overRoute);
    free(this->linkToState);
    free(this->goalToLink);
    free(this->links);
    free(this->goals);
    free(this->states);
}

Route* RoutePrediction::startPrediction(Intersection* currentIntersection, int* currentCondition)
{
	this->predictedGoal = Goal(1, currentCondition);

	GenericMap<int, Link*>* nextLinks = currentIntersection->getOutgoingLinks();

	this->probabilitySize = nextLinks->getSize()*this->goals->getSize();
	this->probabilities = new double[this->probabilitySize];

	// creating the probability of each goal based on its relation to the conditions
	int counter = 1;
	this->goals->initializeCounter();
	GenericEntry<int, Goal*>* nextGoal = this->goals->nextEntry();
	while(nextGoal != NULL)
	{
		nextLinks->initializeCounter();
		GenericEntry<int, Link*>* nextLink = nextLinks->nextEntry();
		while(nextLink != NULL)
		{
			double goalProbability = this->goalToLink->probabilityOfGoalGivenLink(nextLink->value, nextGoal->value, false);
			if(this->predictedGoal.isSimilar(nextGoal->value))
			{
				// high probability since condition is right
				goalProbability *= nextGoal->value->getNumSeen();
			} else {
				// lower probability since condition is wrong
				goalProbability *= .1*nextGoal->value->getNumSeen();
			}

			std::pair<Link*, Goal*>* thisPair = new std::pair<Link*, Goal*>(nextLink->value, nextGoal->value);
			this->states->addEntry(counter, thisPair);
			this->probabilities[counter] = max(this->minInitialProbability*1/this->goals->getSize(), goalProbability);

			counter++;
			nextLink = nextLinks->nextEntry();
		}
		free(nextLink);
		nextGoal = this->goals->nextEntry();
	}
	free(nextGoal);
	free(nextLinks);

	// normalize probabilities
	int sum = 0;
	for(int i = 0; i < this->probabilitySize; i++) { sum += this->probabilities[i]; }
	for(int i = 0; i < this->probabilitySize; i++) { this->probabilities[i] /= sum; }

	this->currentRoute->setToIntersection(currentIntersection);
	this->predictedRoute = predictPrivate(NULL, this->states->copy(), copyProbs());
	return createRouteConditions(currentCondition);
}

double* RoutePrediction::copyProbs()
{
	double* copy = new double[this->probabilitySize];
	for(int i = 0; i < this->probabilitySize; i++)
	{
		copy[i] = this->probabilities[i];
	}
	return copy;
}

Route* RoutePrediction::predict(Link* linkTaken)
{
	GenericMap<int, Link*>* legalLinks;
	if(this->currentRoute->isIntersection())
	{
		legalLinks = this->currentRoute->getIntersection()->getOutgoingLinks();
		free(this->currentRoute);
	} else {
		assert(!linkTaken->isFinalLink());
		legalLinks = this->city->getNextLinks(this->currentRoute->getLastLink());
	}

	// make sure that the link given is legal
	bool error = true;
	legalLinks->initializeCounter();
	GenericEntry<int, Link*>* nextLegalLink = legalLinks->nextEntry();
	while(nextLegalLink != NULL)
	{
		if(nextLegalLink->value->isEqual(linkTaken))
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
	} else if (linkTaken->isFinalLink()) {
		this->predictedRoute = this->overRoute;
		return this->overRoute;
	}

	updateStates(linkTaken, this->states, this->probabilities);
	this->currentRoute->addLink(linkTaken);

	if(linkTaken->isEqual(this->predictedRoute->getEntry(0)))
	{
		this->predictedRoute->removeFirstLink();
	} else {
		this->predictedRoute = predictPrivate(NULL, this->states->copy(), copyProbs());
	}

	if(this->predictedRoute->isEmpty())
	{
		return this->predictedRoute;
	} else if (this->predictedRoute->getLinkSize() == 1) {
		return createRouteIntersection(this->city->getIntersectionFromLink(linkTaken, true), this->predictedGoal.getBins());
	} else {
		return createRoute();
	}
}

void RoutePrediction::updateStates(Link* chosenLink, GenericMap<int, pair<Link*,Goal*>*>* currentStates, double* currentProbabilities)
{
	// get next links
	GenericMap<int, Link*>* nextLinks = this->city->getNextLinks(chosenLink);

	// generate new data structures
	GenericMap<int, pair<Link*,Goal*>*> newStates;
	int newProbabilitySize = sizeof(*nextLinks)/sizeof(Link)*this->goals->getSize();
	double* newProbabilities = new double[newProbabilitySize];

	// generate reused fields
	int* hash;
	Link* li;
	Goal* gi;
	Goal* gj;
	double pSi, pGl, pLs, minProbability;
	pair<Link*, Goal*>* sj;
	int counter = 0;

	// calculate new states and probabilities
	nextLinks->initializeCounter();
	GenericEntry<int, Link*>* nextLink = nextLinks->nextEntry();
	while(nextLink != NULL)
	{
		this->goals->initializeCounter();
		GenericEntry<int, Goal*>* nextGoal = this->goals->nextEntry();
		li = nextLink->value;
		while(nextGoal != NULL)
		{
			pSi = 0;
			gi = nextGoal->value;
			for(int j = 0; j < currentStates->getSize(); j++)
			{
				sj = currentStates->getEntry(j);
				if(sj->first->isEqual(li))
				{
					gj = sj->second;
					minProbability = this->minInitialProbability / this->goals->getSize();
					pGl = this->goalToLink->probabilityOfGoalGivenLink(li, gi, 0);
					pLs = this->linkToState->getProbability(li, chosenLink, gj, false);
					pSi += max(minProbability,currentProbabilities[j])*max(minProbability,pLs)*max(minProbability,pGl);
				}
			}
			pair<Link*, Goal*> si (nextLink->value, gi);
			newStates.addEntry(counter, &si);
			newProbabilities[counter] = pSi;
			nextGoal = this->goals->nextEntry();
		}
		nextLink = nextLinks->nextEntry();
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

Route* RoutePrediction::predictPrivate(Route* currentRoute, GenericMap<int, pair<Link*,Goal*>*>* currentStates, double* currentProbabilities)
{
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
	(*currentRoute).addLink(nextState->first);

	if(!(*nextState->first).isFinalLink())
	{
		updateStates(nextState->first, currentStates, currentProbabilities);
		return predictPrivate(currentRoute, currentStates, currentProbabilities);
	}
	free(currentStates); free(currentProbabilities);
	return currentRoute;
}

Route* RoutePrediction::createRoute()
{
	return createRouteConditions(this->predictedGoal.getBins());
}

Route* RoutePrediction::createRouteConditions(int* currentConditions)
{
	int lastLinkIndex = this->predictedRoute->getLinkSize() - 1;
	return createRouteIntersection(this->city->getIntersectionFromLink(this->predictedRoute->getEntry(lastLinkIndex), true), currentConditions);
}

Route* RoutePrediction::createRouteIntersection(Intersection* intersection, int* currentConditions)
{
	free(&this->predictedGoal);
	this->predictedGoal = Goal(intersection->getIntersectionID(), currentConditions);
	Route route(this->predictedRoute->getLinks(), &this->predictedGoal);
	return &route;
}

void RoutePrediction::parseRoute(Route* route)
{
	// get hash of route goal and add it to goals if nonexistent
	int goalHash = route->getGoalHash();
	if(this->goals->hasEntry(goalHash))
	{
		this->goals->getEntry(goalHash)->incrementNumSeen();
	} else {
		route->getGoal()->setNumSeen(1);
		this->goals->addEntry(goalHash, route->getGoal());
	}

	// add links to link map
	for(int i = 0; i < route->getLinkSize(); i++)
	{
		Link* li; Link* lj;
		lj = route->getLinks()->getEntry(i);

		if(i == route->getLinkSize() - 1)
		{
			li = this->link.finalLink();
		} else {
			li = route->getLinks()->getEntry(i+1);
		}

		this->linkToState->incrementTransition(lj, route->getGoal(), li);
		this->goalToLink->linkTraversed(lj, route->getGoal());
		this->links->addEntry(lj->getHash(), lj);
	}
}

} /* namespace PredictivePowertrain */
