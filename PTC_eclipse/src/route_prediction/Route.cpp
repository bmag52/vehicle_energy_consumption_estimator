/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"
#include "Goal.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace RoutePrediction {

Route::Route() {
	this->counter = 0;
}

Route::Route(Link* links, Goal* goal) {
	this->links = links;
	this->goal = goal;
}

// adds new link to end of route
void Route::addlink(Link* link) {
	int length = getLinkSize();
	Link temp[] = new Link[length + 1];
	for(int i = 0; i < length; i++) {
		temp[i] = this->links[i];
	}
	temp[length] = *link;
}

// checks if the route is equal to the route passed in
bool Route::isEqual(Route other) {
	int myLength = getLinkSize();
	int otherLength = other.getLinkSize();
	if((*(this->goal)).isEqual(other.goal) && myLength == otherLength) {
		for(int i = 0; i < myLength; i++) {
			if(!(this->links[i].isEqual(other.links[i]))) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

//returns copy of current route
Route Route::copy() {
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
	return sizeof(*this->links)/sizeof(Link);
}

Link* Route::getLinksPtr() {
	return this->links;
}

//returns next link in the iterator, returns 0 if there are no more links left
Link Route::nextlink() {
	int length = sizeof(this->links) / sizeof(Link);
	if(this->counter > length) {
		return this->links[length-1]; // return last link
	} else {
		Link link = this->links[this->counter];
		this->counter++;
		return link;
	}
}

}
