/*
 * DataManagement.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#ifndef DATA_MANAGEMENT_DATAMANAGEMENT_H_
#define DATA_MANAGEMENT_DATAMANAGEMENT_H_

#include "../route_prediction/Route.h"
#include "../city/Intersection.h"
#include "../city/Road.h"
#include "../city/City.h"
#include "../map/GenericMap.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "boost/lexical_cast.hpp"

#include <tuple>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

namespace PredictivePowertrain {

class DataManagement {
private:
	std::string routeData = "routes.json";
	std::string cityData = "cities.json";
	std::string tripData = "tripLogs.json";

public:
	DataManagement();
	void addRouteData(Route* route);
	void addCityData(City* city);
	void addTripData(GenericMap<double, double>* latLon, bool logSameDay);
	GenericMap<int, Route*>* getRoutes();
	City* getCityData();
	GenericMap<double, double>* getMostRecentTripData();
};

} /* namespace PredictivePowertrain */


#endif /* DATA_MANAGEMENT_DATAMANAGEMENT_H_ */
