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



namespace PredictivePowertrain {

class CityObj {
public:
	Intersection* intersections;
	std::string roadFileName;
	std:: string intersectionFileName;
	int intervalDistance = 1;
	int dateTimeCreated;
	std::map<int,int> intersectionNumMap;
	Road* roadList;

	CityObj();
	CityObj(Intersection*, std::string, std::string, int, std::map<int, int>);
	int getRoadListSize();
	int getInstersectionListSize();
	Link* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link);
	virtual ~CityObj();
};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
