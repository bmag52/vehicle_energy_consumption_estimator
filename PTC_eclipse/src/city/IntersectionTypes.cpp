/*
 * IntersectionTypes.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "IntersectionTypes.h"
#include <cstdlib>

namespace PredictivePowertrain {

IntersectionTypes::IntersectionTypes() {
	min_stop_time = 0.0;
	max_stop_time = 0.0;
	toString = "";

}

IntersectionTypes::IntersectionTypes(toString, min_stop_time, max_stop_time) {
	this->toString = toString;
	this->min_stop_time = min_stop_time;
	this->max_stop_time = max_stop_time;

}

//assuming the intersectionType is default, stop light, stop sign, destination, exit
//any other mispelling is the fault of user
IntersectionTypes::IntersectionTypes(std::string intersectionType) {
	switch(intersectionType) {
	case "default":
		//%no type specified
		min_stop_time = 0.0;
		max_stop_time = 10.0;
		toString = "Intersection Type: " + intersectionType + " Min Stop Time: " + min_stop_time
					+ " Max Stop Time: " + max_stop_time;
		break;
	case "stop light":
		//%intersection has a light, may have to stop, not guaranteed
		min_stop_time = 0;
		max_stop_time = 30;
		toString = "Intersection Type: " + intersectionType + " Min Stop Time: " + min_stop_time
					+ " Max Stop Time: " + max_stop_time;
		break;
	case "stop sign":
		//%have to stop, this intersection object is a stop sign
		min_stop_time = 5;
		max_stop_time = 10;
		toString = "Intersection Type: " + intersectionType + " Min Stop Time: " + min_stop_time
					+ " Max Stop Time: " + max_stop_time;
		break;
	case "destination":
		//%destination intersection object, only stop if end of route
		//%only stop here if the is the end point, so min/max stop time is 0,
        //because stop here at the end of the route, so vehicle must stop
		min_stop_time = 0;
		max_stop_time = 0;
		toString = "Intersection Type: " + intersectionType + " Min Stop Time: " + min_stop_time
					+ " Max Stop Time: " + max_stop_time;
		break;
	case "exit":
		//%do not stop if continuing on same type of road, but not reversing direction, possible stop otherwise
		min_stop_time = 0;
		max_stop_time = 10;
		toString = "Intersection Type: " + intersectionType + " Min Stop Time: " + min_stop_time
					+ " Max Stop Time: " + max_stop_time;
		break;
	}
}

//returns how long the vehicle should wait at this intersection,
//given either the road types or the road themselves
double IntersectionTypes::stop_time(std::string type, IntersectionTypes prev, IntersectionTypes next) {
	if((type.compare("exit") == 0) && prev.min_stop_time == next.min_stop_time && prev.max_stop_time == next.max_stop_time) {
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
