/*
 * ElevationTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_ELEVATIONTYPES_H_
#define CITY_ELEVATIONTYPES_H_

namespace PredictivePowertrain {

class ElevationTypes {
public:
	int grade;
	int deviation;
	double max_section_length;
	double min_section_length;
//	std::string toString;
//	ElevationTypes(std::string);
	virtual ~ElevationTypes();
};

} /* namespace PredictivePowertrain */

#endif /* CITY_ELEVATIONTYPES_H_ */
