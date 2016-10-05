/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace PredictivePowertrain {
    
Route::~Route()
{
    delete(this->links);
    delete(this->intersection);
}

Route::Route()
{
	this->linkCount = 0;
    this->links = new GenericMap<int, Link*>();
    this->intersection = new Intersection();
}

Route::Route(GenericMap<int, Link*>* links, Goal* goal) {
	this->links = links;
	this->goal = goal;
	this->linkCount = links->getSize();
	Link error(NULL, -1);
	this->error = &error;
}

// adds new link to end of route
void Route::addLink(Link* link) {
	this->links->addEntry(this->linkCount, link);
    this->linkCount++;
}

// checks if the route is equal to the route passed in
bool Route::isEqual(Route * other)
{
	if(this->getLinkSize() == other->getLinkSize()) {
		GenericMap<int, Link*>* otherLinks = other->getLinks();

        for(int i = 0; i < this->getLinkSize(); i++)
        {
            Link* thisLink = this->links->getEntry(i);
            Link* otherLink = otherLinks->getEntry(i);
            
            if(!thisLink->isEqual(otherLink))
            {
                return false;
            }
        }
        
		return true;
	} else {
		return false;
	}
}

Route* Route::copy() {
	Goal* newGoal = new Goal(this->goal);
	GenericMap<int, Link*>* newLinks = this->links->copy();
	Route* route = new Route(newLinks, newGoal);
	return route;
}

int Route::getGoalHash() {
	return this->goal->getHash();
}

Goal* Route::getGoal() {
	return this->goal;
}
    
void Route::assignGoal(Goal* goal)
{
    this->goal = goal;
}

int Route::getLinkSize() {
	return this->links->getSize();
}

GenericMap<int, Link*>* Route::getLinks() {
	return this->links;
}

void Route::setToIntersection(Intersection* other) {
	this->intersection = other;
	this->routeIsIntersection = true;
}
    
void Route::setToRoute() {
    this->intersection = NULL;
    this->routeIsIntersection = false;
}

bool Route::isIntersection() {
	return this->routeIsIntersection;
}

Intersection* Route::getIntersection() {
	return this->intersection;
}

Link* Route::getLastLink() {
	return this->links->getEntry(this->linkCount - 1);
}

bool Route::isEmpty() {
	return this->links->getSize() == 0;
}

Link* Route::getEntry(int index) {
	if(index > this->linkCount - 1)
	{
		return this->error;
	}
	return this->links->getEntry(index);
}

void Route::removeFirstLink() {
	if(this->links->getSize() > 0)
	{
        this->links->indexErase(0);
	}
    this->linkCount--;
}
    
void Route::printLinks()
{
    this->links->initializeCounter();
    GenericEntry<int, Link*>* nextLink = this->links->nextEntry();
    while(nextLink != NULL)
    {
        std::cout << nextLink->value->getNumber() << " | ";
        nextLink = this->links->nextEntry();
    }
    std::cout << std::endl;
    
    delete(nextLink);
}

}
