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
#include <ctime>
#include <vector>

namespace PredictivePowertrain {

class City {
private:
	Link* link;
	int intervalDistance = 1;
	int dateTimeCreated;
	double maxSlopePercent = 6;
	GenericMap<long int, Road*>* roads;
	GenericMap<long int, Intersection*>* intersections;
	GenericMap<int, Bounds*>* boundsMap;
    std::vector<float>* reverseTrace(std::vector<float>* trace);
	Road* getConnectingRoad(Intersection* one, Intersection* two);
    std::pair<std::vector<float>*, float>* elevationToSlope(std::vector<float>* elev, float oldElev);
public:
	City();
	City(GenericMap<long int, Intersection*>* intersections, GenericMap<long int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) ;
    virtual ~City();
	int getRoadMapSize();
	int getInstersectionMapSize();
	int getBoundsMapSize();
	GenericMap<int, Link*>* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link, bool isIntersection);
    Route* randomPath(Intersection* startInt, Route* initialRoute, int totalLength, std::vector<float>* conditions);
	Intersection* getIntersection(int intersectionNum);
    Route* getPath(Intersection* start, Intersection* end, std::vector<float>* conditions, int fastest);
    std::vector<float>* getElevData(Link* link);
	bool legalRoute(Route* route);
    std::vector<float>* routeToElevData(Route* route, int dist);
	GenericMap<int, Bounds*>* getBoundsMap();
	GenericMap<long int, Road*>* getRoads();
	GenericMap<long int, Intersection*>* getIntersections();

};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
