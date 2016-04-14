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
#include "../map/GenericMap.h"
#include "../driver_prediction/Link.h"
#include "../route_prediction/Route.h"

#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <climits>
#include <math.h>

namespace PredictivePowertrain {

class City {
private:
	Intersection* intersections;
	std::string roadFileName;
	std:: string intersectionFileName;
	int intervalDistance = 1;
	int dateTimeCreated;
	int maxSlopePercent = 6;
	std::map<int,int> intersectionNumMap;
	Road* roadList;
	Link* link;

	int* reverseTrace(int* trace);
	Road* getConnectingRoad(Intersection* one, Intersection* two);
	Route* randomPath(Intersection* startInt, Route* initialRoute, int totalLength, int* conditions);
	Link* addLink(Link* links, Link* link);
	Intersection* addIntersection(Intersection* intersections, Intersection* intersection);
	std::pair<double*, int>* elevationToSlope(int* elev, int oldElev);
public:
	City();
	City(Intersection*, std::string, std::string, int, std::map<int, int>);
	int getRoadListSize();
	int getInstersectionListSize();
	Link* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link, bool isIntersection);
	Intersection* getIntersection(int intersectionNum);
	Route* getPath(Intersection* start, Intersection* end, int* conditions, int fastest);
	virtual ~City();
	std::pair<int*, int*>* getRoadData(Link* link);
	bool legalRoute(Route* route);
	std::pair<int*, double*>* routeToData(Route* route, int dist);

};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
