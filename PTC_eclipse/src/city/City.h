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
#include "../data_management/Bounds.h"

#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <climits>
#include <math.h>

namespace PredictivePowertrain {

class City {
private:
	Link* link;
	std::string roadFileName;
	std::string intersectionFileName;
	int intervalDistance = 1;
	int dateTimeCreated;
	int maxSlopePercent = 6;
	std::map<int,int> intersectionNumMap;
	GenericMap<int, Road*>* roads;
	GenericMap<int, Intersection*>* intersections;
	GenericMap<int, Bounds*>* boundsMap;

	int* reverseTrace(int* trace);
	Road* getConnectingRoad(Intersection* one, Intersection* two);
	Route* randomPath(Intersection* startInt, Route* initialRoute, int totalLength, int* conditions);
	Intersection* addIntersection(Intersection* intersections, Intersection* intersection);
	std::pair<double*, int>* elevationToSlope(int* elev, int oldElev);
public:
	City();
	City(GenericMap<int, Intersection*>* intersections, GenericMap<int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) ;
	int getRoadListSize();
	int getInstersectionListSize();
	GenericMap<int, Link*>* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link, bool isIntersection);
	Intersection* getIntersection(int intersectionNum);
	Route* getPath(Intersection* start, Intersection* end, int* conditions, int fastest);
	virtual ~City();
	std::pair<int*, int*>* getRoadData(Link* link);
	bool legalRoute(Route* route);
	std::pair<int*, double*>* routeToData(Route* route, int dist);
	GenericMap<int, Bounds*>* getBoundsMap();
	GenericMap<int, Road*>* getRoads();
	GenericMap<int, Intersection*>* getIntersections();

};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
