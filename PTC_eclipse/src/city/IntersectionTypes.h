/*
 * IntersectionTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_INTERSECTIONTYPES_H_
#define CITY_INTERSECTIONTYPES_H_

namespace PredictivePowertrain {

class IntersectionTypes {
public:
//	std::string toString;
	double min_stop_time; //time in seconds
	double max_stop_time; //time in seconds
//	IntersectionTypes(std::string);
//	double stop_time(std::string, IntersectionTypes, IntersectionTypes);
	IntersectionTypes();
	virtual ~IntersectionTypes();
};

} /* namespace PredictivePowertrain */

#endif /* CITY_INTERSECTIONTYPES_H_ */
