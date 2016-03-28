/*
 * CityObj.h
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#ifndef CITY_CITY_H_
#define CITY_CITY_H_
#define  max_slope_percent 6

#include <string>
#include <map>

#include "Intersection.h"
#include "Road.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace City {

class CityObj {
public:
	Intersection* intersections;
	int dateTimeCreated;
	std::string roadFileName;
	std:: string intersectionFileName;
	int intervalDistance = 1;
	std::map<int,int> intersectionNumMap;
	Road* road_list;
	CityObj(Intersection*, std::string, std::string, int, std::map<int, int>);
	CityObj();
	int getRoadListSize();
	int getInstersectionListSize();
	Link* getNextLinks(Link* link);
	Link* linkFromRoad(Road* road, Intersection* intersection);
	Intersection* getIntersectionFromLink(Link* link);
	virtual ~CityObj();
};

} /* namespace City */

#endif /* CITY_CITY_H_ */
