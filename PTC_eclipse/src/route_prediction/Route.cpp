/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Goal.h"
#include "../driver_prediction/Link.h"
#include "../city/Intersection.h"

using namespace DriverPrediction;
using namespace City;

namespace RoutePrediction {

class Route {
private:
	Link* links;
	Goal* goal;
	Link* error;
	Intersection* intersection;
	bool goalIsIntersection;
	int counter;
public:
	Route();
	Route(Link* links, Goal* goal);
	void addlink(Link* link);
	bool isequal(Route other);
	int getGoalHash();
	int getLinkSize();
	Link* getEntry(int index);
	void setToIntersection(Intersection* other);
	bool isIntersection();
	bool isEmpty();
	Goal* getGoalPtr();
	Link* getLinksPtr();
	Intersection* getIntersectionPtr();
	Link* getLastLinkPtr();;
	Route copy();
	void removeFirstLink();
	Link nextlink();
};

<<<<<<< HEAD
}

=======
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
	this->counter++;
	Link temp[counter];
	for(int i = 0; i < counter - 1; i++) {
		temp[i] = this->links[i];
	}
	temp[counter - 1] = *link;
	this->links = temp;
}

// checks if the route is equal to the route passed in
bool Route::isequal(Route * other) {
	int myLength = getLinkSize();
	int otherLength = other->getLinkSize();
	if((*(this->goal)).isEqual(other->goal) && myLength == otherLength) {
		for(int i = 0; i < myLength; i++) {
			if(!(this->links[i].isEqual(&(other->links[i])))) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

Route* Route::copy() {
	Goal goal = new Goal(this->goal);
	Link newLinks[this->counter];
	int length = sizeof(newLinks);
	for(int i = 0; i < length; i++) {
		int number = this->links[i].getNumber();
		int direction = this->links[i].getDirection();
		Link * newLink = new Link(direction, number);
		newLinks[i] = *newLink;
	}
	Route* route = new Route(newLinks, &goal);
	return route;
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
>>>>>>> origin/master
