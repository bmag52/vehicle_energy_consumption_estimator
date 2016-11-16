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
    this->city = new City();
    initialize();
}

RoutePrediction::RoutePrediction(City* city)
{
	this->city = city;
    initialize();
}
    
void RoutePrediction::initialize()
{
    Link* unknownLink = new Link(-2, -2);
    Link* overLink = new Link(-3,-3);
    Goal* unknownGoal = new Goal(-1);
    Goal* overGoal = new Goal(1);
    
    this->unknownRoute = new Route(new GenericMap<long int, Link*>(), unknownGoal);
    this->unknownRoute->addLink(unknownLink);
    
    this->overRoute = new Route(new GenericMap<long int, Link*>(), overGoal);
    this->overRoute->addLink(overLink);
    
    this->linkToState = new LinkToStateMap();
    this->goalToLink = new GoalToLinkMap();
    this->links = new GenericMap<long int, Link*>();
    this->goals = new GenericMap<long int, Goal*>();
    this->states = new GenericMap<int, std::pair<Link*, Goal*>*>();
    this->predictedRoute = new Route();
    this->currentRoute = new Route();
    this->predictedGoal = unknownGoal;
    
    if(this->city->getInstersectionMapSize() != 0)
    {
        this->minInitialProbability = 0.1 / this->city->getInstersectionMapSize();
    } else {
        this->minInitialProbability = 0;
    }
}
    
RoutePrediction::~RoutePrediction()
{
    delete(this->unknownRoute);
    delete(this->overRoute);
    delete(this->linkToState);
    delete(this->goalToLink);
    delete(this->links);
    delete(this->goals);
    delete(this->states);
    delete(this->predictedRoute);
    delete(this->currentRoute);
    delete(this->predictedGoal);
}
    
GenericMap<long int, Link*>* RoutePrediction::getLinks()
{
    return this->links;
}

GenericMap<long int, Goal*>* RoutePrediction::getGoals()
{
    return this->goals;
}

LinkToStateMap* RoutePrediction::getLinkToState()
{
    return this->linkToState;
}

GoalToLinkMap* RoutePrediction::getGoalToLink()
{
    return this->goalToLink;
}

Route* RoutePrediction::startPrediction(Intersection* currentIntersection, std::vector<float>* currentCondition)
{
    delete(this->predictedGoal);
	this->predictedGoal = new Goal(1, currentCondition);

	GenericMap<long int, Link*>* nextLinks = currentIntersection->getOutgoingLinks();
    this->probabilities = new std::vector<float>(nextLinks->getSize() * this->goals->getSize());

	// creating the probability of each goal based on its relation to the conditions
	int counter = 0;
	this->goals->initializeCounter();
	GenericEntry<long int, Goal*>* nextGoal = this->goals->nextEntry();
	while(nextGoal != NULL)
	{
		nextLinks->initializeCounter();
		GenericEntry<long int, Link*>* nextLink = nextLinks->nextEntry();
		while(nextLink != NULL)
		{
			float goalProbability = this->goalToLink->probabilityOfGoalGivenLink(nextLink->value, nextGoal->value, false);
			if(this->predictedGoal->isSimilar(nextGoal->value))
			{
				// high probability since condition is right
				goalProbability *= (float)nextGoal->value->getNumSeen();
			} else {
				// lower probability since condition is wrong
				goalProbability *= .1 * (float)nextGoal->value->getNumSeen();
			}

			std::pair<Link*, Goal*>* newState = new std::pair<Link*, Goal*>(nextLink->value, nextGoal->value);
			this->states->addEntry(counter, newState);
            this->probabilities->at(counter) = std::max(this->minInitialProbability / ((float)this->goals->getSize()), goalProbability);
			counter++;
            
			nextLink = nextLinks->nextEntry();
		}
		delete(nextLink);
		nextGoal = this->goals->nextEntry();
	}
	delete(nextGoal);

	// normalize probabilities
	float sum = 0;
	for(int i = 0; i < this->probabilities->size(); i++) { sum += this->probabilities->at(i); }
    if(sum != 0)
    {
        for(int i = 0; i < this->probabilities->size(); i++) { this->probabilities->at(i) /= sum; }
    }

	this->currentRoute->setToIntersection(currentIntersection);

    std::vector<float>* probabilitiesCopy = new std::vector<float>(this->probabilities->size());
    for(int i = 0; i < this->probabilities->size(); i++) { probabilitiesCopy->at(i) = this->probabilities->at(i); }
    
    this->predictedRoute = predictPrivate(NULL, this->states->copy(), probabilitiesCopy);
    if(this->predictedRoute->isEqual(this->unknownRoute))
    {
        return this->unknownRoute;
    }
    else if(this->predictedRoute->isEqual(this->overRoute))
    {
        return this->overRoute;
    }
	return createRouteConditions(currentCondition);
}

Route* RoutePrediction::predict(Link* linkTaken)
{
	GenericMap<long int, Link*>* legalLinks;
	if(this->currentRoute->isIntersection())
	{
		legalLinks = this->currentRoute->getIntersection()->getOutgoingLinks();
		this->currentRoute = new Route();
	}
    else
    {
		assert(!linkTaken->isFinalLink());
		legalLinks = this->city->getNextLinks(this->currentRoute->getLastLink());
	}

	// make sure that the link given is legal
	bool error = true;
	legalLinks->initializeCounter();
	GenericEntry<long int, Link*>* nextLegalLink = legalLinks->nextEntry();
	while(nextLegalLink != NULL)
	{
		if(nextLegalLink->value->isEqual(linkTaken))
		{
			error = false;
			break;
		}
        nextLegalLink = legalLinks->nextEntry();
	}
    delete(nextLegalLink);

	// there is an error and the current route is known (ie, wasnt an error before)
    // if not not legal, stop and return an error value
	if(error && this->predictedRoute->isEmpty())
	{
		this->predictedRoute = this->unknownRoute;
		return this->unknownRoute;
	}
    else if (linkTaken->isFinalLink())
    {
		this->predictedRoute = this->overRoute;
		return this->overRoute;
	}

	std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>* update = updateStates(linkTaken, this->states, this->probabilities);
    this->states = update->first;
    this->probabilities = update->second;
    
	this->currentRoute->addLink(linkTaken);

	if(linkTaken->isEqual(this->predictedRoute->getEntry(0)))
	{
		this->predictedRoute->removeFirstLink();
	}
    else
    {
        std::vector<float>* probabilitiesCopy = new std::vector<float>(this->probabilities->size());
        for(int i = 0; i < this->probabilities->size(); i++) { probabilitiesCopy->at(i) = this->probabilities->at(i); }
        
		this->predictedRoute = predictPrivate(NULL, this->states->copy(), probabilitiesCopy);
	}

	if(this->predictedRoute->isEmpty())
	{
		return this->predictedRoute;
    }
    else if(this->predictedRoute->isEqual(this->unknownRoute))
    {
        return this->unknownRoute;
    }
    else if(this->predictedRoute->isEqual(this->overRoute))
    {
        return this->overRoute;
    }
    else if (this->predictedRoute->getLinkSize() == 1)
    {
		return createRouteIntersection(this->city->getIntersectionFromLink(linkTaken, true), this->predictedGoal->getBins());
	}
    else
    {
		return createRoute();
	}
}

std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>*
RoutePrediction::updateStates(Link* chosenLink, GenericMap<int, std::pair<Link*,Goal*>*>* oldStates, std::vector<float>* oldProbabilites)
{
	// get next links
	GenericMap<long int, Link*>* nextLinks = this->city->getNextLinks(chosenLink);

	// generate new data structures
    GenericMap<int, std::pair<Link*, Goal*>*>* newStates = new GenericMap<int, std::pair<Link*, Goal*>*>();
    std::vector<float>* newProbabilities = new std::vector<float>(nextLinks->getSize() * this->goals->getSize());
    int counter = 0;

	// generate reused fields
	Link* li;
	Goal* gi;
	Goal* gj;
	float pSi, pGl, pLs, minProbability;
    std::pair<Link*, Goal*>* sj;

	// calculate new states and probabilities
	nextLinks->initializeCounter();
	GenericEntry<long int, Link*>* nextLink = nextLinks->nextEntry();
	while(nextLink != NULL)
	{
		this->goals->initializeCounter();
		GenericEntry<long int, Goal*>* nextGoal = this->goals->nextEntry();
        li = nextLink->value;
		while(nextGoal != NULL)
		{
            gi = nextGoal->value;
            std::pair<Link*, Goal*>* si = new std::pair<Link*, Goal*>(li, gi);
			pSi = 0;
			for(int j = 0; j < oldStates->getSize(); j++)
			{
				sj = oldStates->getEntry(j);
				if(sj->first->isEqual(chosenLink))
				{
					gj = sj->second;
					minProbability = this->minInitialProbability / ((float)this->goals->getSize());
					pGl = this->goalToLink->probabilityOfGoalGivenLink(li, gi, 0);
					pLs = this->linkToState->getProbability(li, chosenLink, gj, false);
                    pSi += std::max(minProbability, oldProbabilites->at(j)) * std::max(minProbability, pLs) * std::max(minProbability, pGl);
				}
			}
            // add new state to new states
			newStates->addEntry(counter, si);
			newProbabilities->at(counter) = pSi;
            counter++;
            
			nextGoal = this->goals->nextEntry();
		}
		nextLink = nextLinks->nextEntry();
	}

	// normalize probabilities
	float sum = 0;
	for(int i = 0; i < newProbabilities->size(); i++) { sum += newProbabilities->at(i); }
    if(sum != 0)
    {
        for(int i = 0; i < newProbabilities->size(); i++) { newProbabilities->at(i) /= sum; }
    }

	// update probabilities and states
    std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>* update =
        new std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>(newStates, newProbabilities);
    
    return update;
}

Route* RoutePrediction::predictPrivate(Route* currentRoute, GenericMap<int, std::pair<Link*,Goal*>*>* currentStates, std::vector<float>* currentProbabilities)
{
	// find max prob
    float maxProbability = -1;
    int nextStateIndex = 0;
    
	for(int i = 0; i < currentProbabilities->size(); i++)
    {
        if(currentProbabilities->at(i) > maxProbability)
        {
            maxProbability = currentProbabilities->at(i);
            nextStateIndex = i;
        }
	}
    
    // return unknown route if no probability associated with next goal in route
    if(maxProbability <= 0 || currentStates->getSize() == 0)
    {
        return this->unknownRoute;
    }

	// get state with highest probability and add it to route
    std::pair<Link*, Goal*>* nextState = currentStates->getEntry(nextStateIndex);
    if(currentRoute == NULL)
    {
        currentRoute = new Route();
    }
    currentRoute->addLink(nextState->first);

	if(!nextState->first->isFinalLink())
	{
        std::pair<GenericMap<int, std::pair<Link*,Goal*>*>*, std::vector<float>*>* update = updateStates(nextState->first, currentStates, currentProbabilities);
        
        delete(currentStates);
        delete(currentProbabilities);
        
		return predictPrivate(currentRoute, update->first, update->second);
	}
	return currentRoute;
}

Route* RoutePrediction::createRoute()
{
	return createRouteConditions(this->predictedGoal->getBins());
}

Route* RoutePrediction::createRouteConditions(std::vector<float>* currentConditions)
{
    int lastLinkIndex = std::max(this->predictedRoute->getLinkSize() - 2, 0);
	return createRouteIntersection(this->city->getIntersectionFromLink(this->predictedRoute->getEntry(lastLinkIndex), true), currentConditions);
}

Route* RoutePrediction::createRouteIntersection(Intersection* intersection, std::vector<float>* currentConditions)
{
	delete(this->predictedGoal);
	this->predictedGoal = new Goal(intersection->getIntersectionID(), currentConditions);
    this->predictedRoute->assignGoal(predictedGoal);
	Route* route = new Route(this->predictedRoute->getLinks(), this->predictedGoal);
	return route;
}

void RoutePrediction::parseRoute(Route* route)
{
	// get hash of route goal and add it to goals if nonexistent
	long int goalHash = route->getGoalHash();
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
    
void RoutePrediction::addPredictionElements(GenericMap<long int, Link*>* newLinks,
                                            GenericMap<long int, Goal*>* newGoals,
                                            GoalToLinkMap* newGoalToLink,
                                            LinkToStateMap* newLinkToState)
{
    delete(this->links);
    this->links = newLinks;
    
    delete(this->goals);
    this->goals = newGoals;
    
    delete(this->goalToLink);
    this->goalToLink = newGoalToLink;
    
    delete(this->linkToState);
    this->linkToState = newLinkToState;
}
void RoutePrediction::addCity(City* newCity)
{
    if(this->city != NULL)
    {
        delete(this->city);
    }
    this->city = newCity;
}
    
Route* RoutePrediction::getPredictedRoute()
{
    return this->predictedRoute->copy();
}

City* RoutePrediction::getCity()
{
    return this->city;
}
    
Route* RoutePrediction::getUnknownRoute()
{
    return this->unknownRoute;
}
    
Route* RoutePrediction::getOverRoute()
{
    return this->overRoute;
}
    
} /* namespace PredictivePowertrain */
