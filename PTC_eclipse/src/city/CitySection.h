/*
 * CitySection.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_CITYSECTION_H_
#define CITY_CITYSECTION_H_

#include <string>
#include "RoadTypes.h"

namespace PredictivePowertrain {

class CitySection {
public:
	std::string roadType;
	int x_dim;
	int y_dim;
	CitySection();
	CitySection(std::string, int, int);
	virtual ~CitySection();
};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITYSECTION_H_ */
