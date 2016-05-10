/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace PredictivePowertrain {

Route::Route() {
	this->linkCount = 0;
}

Route::Route(GenericMap<int, Link*>* links, Goal* goal) {
	this->links = links;
	this->goal = goal;
	//this->counter = 0;
	this->linkCount = links->getSize();
	Link error(NULL, -1);
	this->error = &error;
}

// adds new link to end of route
void Route::addlink(Link* link) {
	this->linkCount++;
	this->links->addEntry(this->linkCount, link);
}

// checks if the route is equal to the route passed in
bool Route::isequal(Route * other) {
	if(this->getLinkSize() == other->getLinkSize()) {

		GenericMap<int, Link*>* otherLinks = other->getLinksPtr();
		this->links->initializeCounter();
		GenericEntry<int, Link*>* nextLink = this->links->nextEntry();

		while(nextLink != NULL)
		{
			if(!otherLinks->hasEntry(nextLink->key)) {
				return false;
			}
			nextLink = this->links->nextEntry();
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

Goal* Route::getGoalPtr() {
	return this->goal;
}

int Route::getLinkSize() {
	return this->links->getSize();
}

GenericMap<int, Link*>* Route::getLinksPtr() {
	return this->links;
}

void Route::setToIntersection(Intersection* other) {
	this->intersection = other;
	this->goalIsIntersection = true;
}

bool Route::isIntersection() {
	return this->goalIsIntersection;
}

Intersection* Route::getIntersectionPtr() {
	return this->intersection;
}

Link* Route::getLastLinkPtr() {
	return this->links->getEntry(this->linkCount);
}

bool Route::isEmpty() {
	return this->links->getSize() == 0;
}

Link* Route::getEntry(int index) {
	if(index > this->linkCount)
	{
		return this->error;
	}
	return this->links->getEntry(index);
}

void Route::removeFirstLink() {
	if(this->links->getSize() > 0)
	{
		this->links->erase(1);
		GenericMap<int, Link*>* newLinks = new GenericMap<int, Link*>();

		this->links->initializeCounter();
		GenericEntry<int, Link*>* nextLink = this->links->nextEntry();
		while(nextLink != NULL)
		{
			newLinks->addEntry(nextLink->key-1, nextLink->value);
			nextLink = this->links->nextEntry();
		}
		free(this->links);
		this->links = newLinks;
	}
}

}
