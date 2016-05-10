/*
 * IntersectionTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_INTERSECTIONTYPES_H_
#define CITY_INTERSECTIONTYPES_H_

#include <cstdlib>
#include <string>

namespace PredictivePowertrain {

class IntersectionTypes {
public:
	std::string toString;
	double min_stop_time; //time in seconds
	double max_stop_time; //time in seconds
	double stop_time(std::string type, IntersectionTypes* prev, IntersectionTypes* next);
	IntersectionTypes();
	IntersectionTypes(std::string toString, double min_stop_time, double max_stop_time);
	IntersectionTypes(std::string intersectionType);
	virtual ~IntersectionTypes();
};

} /* namespace PredictivePowertrain */

#endif /* CITY_INTERSECTIONTYPES_H_ */
