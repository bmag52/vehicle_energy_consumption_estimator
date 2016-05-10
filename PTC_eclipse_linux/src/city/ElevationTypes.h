/*
 * ElevationTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_ELEVATIONTYPES_H_
#define CITY_ELEVATIONTYPES_H_

#include <string>
#include <iostream>

namespace PredictivePowertrain {

class ElevationTypes {
public:
	int grade;
	int deviation;
	double max_section_length;
	double min_section_length;
	std::string toString;
	ElevationTypes();
	ElevationTypes(int grade, int deviation, double min_section_length, double max_section_length, std::string toString);
	ElevationTypes(std::string);
};

} /* namespace PredictivePowertrain */

#endif /* CITY_ELEVATIONTYPES_H_ */
