/*
 * ElevationTypes.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "ElevationTypes.h"

namespace PredictivePowertrain {

ElevationTypes::ElevationTypes() {
	this->grade = 0;
	this->deviation = 0;
	this->max_section_length = 0.0;
	this->min_section_length = 0.0;
	this->toString = "";
}

ElevationTypes::ElevationTypes(int grade, int deviation, double min_section_length, double max_section_length, std::string toString) {
	this->grade = grade;
	this->deviation = deviation;
	this->min_section_length = min_section_length;
	this->max_section_length = max_section_length;
	this->toString = toString;
}

ElevationTypes::ElevationTypes(std::string typeElevation) {
	if(typeElevation.compare("default")) {
		//%basic qualities
		this->grade = 5;
		this->deviation = 100;
		this->max_section_length = .1;
		this->min_section_length = .7;
	} else if(typeElevation.compare("flat")) {
		//%long sections, low grade
		this->grade = 2;
		this->deviation = 20;
		this->max_section_length = .6;
		this->min_section_length = .7;
	} else if(typeElevation.compare("moderate")) {
		//%medium sections, moderate grade
		this->grade = 5;
		this->deviation = 500;
		this->max_section_length = .3;
		this->min_section_length = .6;
	} else if(typeElevation.compare("hilly")) {
		// %short sections, high grade
		this->grade = 10;
		this->deviation = 100;
		this->max_section_length = .3;
		this->min_section_length = .5;
	} else if(typeElevation.compare("exit")) {
		//%long sections, high grade
		this->grade = 10;
		this->deviation = 50;
		this->max_section_length = .5;
		this->min_section_length = .7;
	}
	this->toString = typeElevation;
}

} /* namespace PredictivePowertrain */
