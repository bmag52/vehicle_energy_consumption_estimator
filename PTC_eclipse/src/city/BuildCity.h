/*
 * BuildCity.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef CITY_BUILDCITY_H_
#define CITY_BUILDCITY_H_

#include "City.h"

namespace PredictivePowertrain {

class BuildCity {
private:
	City city;
	GenericMap<int, Road*> rawRoads;
	GenericMap<int, Intersection*> intersections;
	void makeRoadPolys();
	void makeGridData();

public:
	BuildCity();

};

}

#endif /* CITY_BUILDCITY_H_ */
