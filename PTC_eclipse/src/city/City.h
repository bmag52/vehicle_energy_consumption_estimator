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
#include "../route_prediction/Route.h"

#include <assert.h>

namespace PredictivePowertrain {

class CityObj {
private:
	Intersection* intersections;
	std::string roadFileName;
	std:: string intersectionFileName;
	int intervalDistance = 1;
	int dateTimeCreated;
	std::map<int,int> intersectionNumMap;
	Road* roadList;
	Link* link;

	int* reverseTrace(int* trace);
public:
	CityObj();
	CityObj(Intersection*, std::string, std::string, int, std::map<int, int>);
	int getRoadListSize();
	int getInstersectionListSize();
	Link* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link, bool isIntersection);
	Intersection* getIntersection(int intersectionNum);
	Route* getPath(Intersection* start, Intersection* end, int* conditions, int fastest);
	virtual ~CityObj();
	std::pair<int*, int*>* getRoadData(Link* link);
	bool legalRoute(Route* route);
};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
