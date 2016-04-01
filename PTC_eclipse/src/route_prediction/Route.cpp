/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace PredictivePowertrain {

Route::Route() {
	this->counter = 0;
}

Route::Route(Link* links, Goal* goal) {
	this->links = links;
	this->goal = goal;
	this->counter = 0;
	Link error(-1, -1);
	this->error = &error;
}

// adds new link to end of route
void Route::addlink(Link* link) {
	// todo addlink
//	int length = sizeof(this->links) / sizeof(Link);
//	Link temp[] = new Link[length];
//	for(int i = 0; i < length; i++) {
//		temp[i] = links[i];
//	}
//	temp[length] = link;
//	this->links = temp;
}

// checks if the route is equal to the route passed in
bool Route::isequal(Route other) {
	// todo isequal
//	int mylength = sizeof(this->links) / sizeof(Link);
//	int otherlength = sizeof(other.links) / sizeof(Link);
//	if(!(mylength == otherlength) || !((this->goal).isequal(other.goal))) {
//		return false;
//	}
//	for(int i = 0; i < mylength; i++) {
//		if(!(this->links[i].isEqual(other.links[i]))) {
//			return false;
//		}
//	}
//	return true;
}

//returns copy of current route
Route* Route::copy() {
	// TODO copy
//	Goal goal = Goal(this->goal.destination, this->goal.bins, this->goal.size);	//is this the right? Goal variables are all private, but not sure if there's a better way
//	int length = sizeof(this->links) / sizeof(Link);
//	links = Link[length];
//	for(int i = 0; i < length; i++) {
//		int number = this->links[i].getNumber();
//		int direction = this->links[i].getDirection();
//		links[i] = this->links[i].copy(direction, number);
//	}
//	return Route(links, goal);
}

int Route::getGoalHash() {
	return (*this->goal).getHash();
}

Goal* Route::getGoalPtr() {
	return this->goal;
}

int Route::getLinkSize() {
	return this->counter;
}

Link* Route::getLinksPtr() {
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
	return &this->links[counter];
}

bool Route::isEmpty() {
	return this->counter == 0;
}

Link* Route::getEntry(int index) {
	if(index > this->counter)
	{
		return this->error;
	}
	return &this->links[index];
}

void Route::removeFirstLink() {
	Link * newLinks = new Link[this->counter - 1];
	for(int i = 0; i < this->counter - 1; i++)
	{
		newLinks[i] = this->links[i+1];
	}
	free(this->links);
	this->links = newLinks;
}

//returns next link in the iterator, returns 0 if there are no more links left
Link* Route::nextlink() {
	int length = sizeof(this->links) / sizeof(Link);
	if(this->counter > length) {
		return &this->links[length-1]; // return last link
	} else {
		Link link = this->links[this->counter];
		this->counter++;
		return &link;
	}
}

}


