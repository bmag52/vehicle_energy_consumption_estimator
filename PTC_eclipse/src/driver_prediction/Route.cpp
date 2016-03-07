/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace DriverPrediction {
using namespace InnovationModel;

Route::Route(Link links[], Goal &goal) {
	this->links = links;
	this->goal = goal;
	this->counter = 0;
}

//returns copy of current route
Route Route::copy() {
	goal = Goal(this->goal.destination, this->goal.bins, this->goal.size);
	int length = sizeof(this->links) / sizeof(Link);
	links = Link[length];
	for(int i = 0; i < length; i++) {
		int number = this->links[i].getNumber();
		int direction = this->links[i].getDirection();
		links[i] = this->links[i].copy(number, direction);
	}
	return Route(links, goal);
}

//returns next link in the iterator, returns 0 if there are no more links left
Link Route::nextlink() {
	int length = sizeof(this->links) / sizeof(Link);
	if(this->counter > length || this->counter < 1) {
		return 0;
	} else {
		Link link = this->links[this->counter];
		this->counter++;
		return link;
	}
}
}
