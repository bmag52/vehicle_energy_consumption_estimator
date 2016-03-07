/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace DriverPrediction {
Route::Route(Link links[], Goal &goal) {
	this->links = links;
	this->goal = goal;
	this->counter = 0;
}

// This adds off the end of the existing array, but not sure if that is a safe thing to do
void Route::addlink(Link &link) {
	int length = sizeof(this->links) / sizeof(Link);
	Link temp[] = new Link[length];
	for(int i = 0; i < length; i++) {
		temp[i] = links[i];
	}
	temp[length] = link;
	this->links = temp;
}

bool Route::isequal(Route &other) {
	int mylength = sizeof(this->links) / sizeof(Link);
	int otherlength = sizeof(other.links) / sizeof(Link);
	if(!(mylength == otherlength) || !((this->goal).isequal(other.goal))) {
		return false;
	}
	for(int i = 0; i < mylength; i++) {
		if(!(this->links[i].isEqual(other.links[i]))) {
			return false;
		}
	}
	return true;
}
}
