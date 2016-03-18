 /*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "../route_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include "Route.h"

namespace InnovationModel {

Route::Route(Link links[], Goal& goal) {
	(*this->links) = *links;
	this->goal = goal;
	this->counter = 0;
}

Route::~Route() {
	// TODO Auto-generated destructor stub
}

void Route::addlink(Link link) {
	int length = (sizeof(this->links))/(sizeof(this->links[0]));
	Link temp[length + 1];
	for(int i = 0; i < length; i++) {
	}
	temp[length] = link;
	(*this->links) = (*temp);
}

// checks if the route is equal to the route passed in
bool Route::isequal(Route &other) {
	int mylength = sizeof(this->links) / sizeof(this->links[0]);
	int otherlength = sizeof(other.links) / sizeof(other.links[0]);
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

//returns copy of current route
Route Route::copy() {
	Goal newGoal(this->goal.getDestination(), this->goal.getBins(), this->goal.getSize());
	int length = sizeof(this->links) / sizeof(this->links[0]);
	Link newLinks[length];
	for(int i = 0; i < length; i++) {
		int number = this->links[i].getNumber();
		int direction = this->links[i].getDirection();
		newLinks[i] = this->links[i].copy(direction, number);
	}
	Route* newRoute = new Route(newLinks, newGoal);
	return *newRoute;
}

//returns next link in the iterator, returns 0 if there are no more links left
Link Route::nextlink() {
	int length = sizeof(this->links) / sizeof(Link);
	if(this->counter == length || this->counter < 1) {
		return this->links[this->counter].final_link(); // not sure if this is ok
	} else {
		Link link = this->links[this->counter];
		this->counter++;
		return link;
	}
}

}
