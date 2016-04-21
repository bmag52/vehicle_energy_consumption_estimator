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
#include "../map/GenericMap.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "boost/lexical_cast.hpp"

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
	void addCityData(Road* roads, Intersection* intersections);
	void addTripData(GenericMap<double, double>* latLon, bool logSameDay);
	GenericMap<int, Route*>* getRoutes(int cityClusterNUm);
	std::pair<GenericMap<int, Road*>*, GenericMap<int, Intersection*>*>* getCityData(int cityClusterNum);
	GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* getMostRecentTripData();
};

} /* namespace PredictivePowertrain */


#endif /* DATA_MANAGEMENT_DATAMANAGEMENT_H_ */
