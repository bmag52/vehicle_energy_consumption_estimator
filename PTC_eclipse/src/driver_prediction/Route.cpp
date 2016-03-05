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
	this->links[length] = link;
}

bool Route::isequal(Route &other) {
	int mylength = sizeof(this->links) / sizeof(Link);
	int otherlength = sizeof(other->links) / sizeof(Link); // what is the correct syntax for this?
	if(!(mylength == otherlength) || !((this->goal).isequal(other->goal))) { //syntax??
		return false;
	}
	for(int i = 0; i < mylength; i++) {
		if(!(this->links[i].isEqual(other->links[i]))) { //syntax...
			return false;
		}
	}
	return true;
}
}
