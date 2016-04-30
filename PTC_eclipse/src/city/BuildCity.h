/*
 * BuildCity.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef CITY_BUILDCITY_H_
#define CITY_BUILDCITY_H_

#include "City.h"
#include "../data_management/DataManagement.h"
#include "../data_management/DataCollection.h"
#include "../map/GenericMap.h"

#include "limits.h"
#include "spline.h"
#include <vector>

namespace PredictivePowertrain {

class BuildCity {
private:
	City city;
	GenericMap<int, Intersection*> intersections;
	GenericMap<int, Road*> roads;
	void updateGridData(); // post-processing on shutdown
	void readCityFile(); // pre-processing on startup

public:
	BuildCity();

};

}

#endif /* CITY_BUILDCITY_H_ */
