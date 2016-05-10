/*
 * IntersectionTypes.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "IntersectionTypes.h"

namespace PredictivePowertrain {

IntersectionTypes::IntersectionTypes() {
	this->min_stop_time = 0.0;
	this->max_stop_time = 0.0;
	this->toString = "";

}

IntersectionTypes::IntersectionTypes(std::string toString, double min_stop_time, double max_stop_time) {
	this->toString = toString;
	this->min_stop_time = min_stop_time;
	this->max_stop_time = max_stop_time;
}

//assuming the intersectionType is default, stop light, stop sign, destination, exit
//any other mispelling is the fault of user
IntersectionTypes::IntersectionTypes(std::string intersectionType) {
	if(!intersectionType.compare("default")) {
		//%no type specified
		this->min_stop_time = 0.0;
		this->max_stop_time = 10.0;
	} else if(!intersectionType.compare("stop light")) {
		//%intersection has a light, may have to stop, not guaranteed
		this->min_stop_time = 0;
		this->max_stop_time = 30;
	} else if(!intersectionType.compare("stop sign")) {
		//%have to stop, this intersection object is a stop sign
		this->min_stop_time = 5;
		this->max_stop_time = 10;
	} else if(!intersectionType.compare("destination")) {
		//%destination intersection object, only stop if end of route
		this->min_stop_time = 0;
		this->max_stop_time = 0;
	} else if(!intersectionType.compare("exit")) {
		//%do not stop if continuing on same type of road, but not reversing direction, possible stop otherwise
		this->min_stop_time = 0;
		this->max_stop_time = 10;
	}
	this->toString = intersectionType;
}

//returns how long the vehicle should wait at this intersection,
//given either the road types or the road themselves
double IntersectionTypes::stop_time(std::string type, IntersectionTypes* prev, IntersectionTypes* next) {
	if((type.compare("exit") == 0) && prev->min_stop_time == next->min_stop_time && prev->max_stop_time == next->max_stop_time) {
		return 0.0;
	} else {
		int r = ((double) rand() / (RAND_MAX));
		return r * (max_stop_time - min_stop_time) + min_stop_time;
	}
}

IntersectionTypes::~IntersectionTypes() {
	// TODO Auto-generated destructor stub
}

} /* namespace PredictivePowertrain */
