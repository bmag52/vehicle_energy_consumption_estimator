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
#include "../speed_prediction/SpeedPrediction.h"
#include "../data_management/Bounds.h"

#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <climits>
#include <math.h>
#include <ctime>
#include <vector>
#include <float.h>

namespace PredictivePowertrain {

class GPS;              // forward declaration
class Route;            // forward declaration
class SpeedPrediction;  // forward declaration
    
class City {
    
private:
	Link* link;
	int intervalDistance = 1;
	int dateTimeCreated;
	double maxSlopePercent = 6;
	GenericMap<long int, Road*>* roads;
	GenericMap<long int, Intersection*>* intersections;
	GenericMap<int, Bounds*>* boundsMap;
	Road* getConnectingRoad(Intersection* one, Intersection* two);
    
public:
	City();
	City(GenericMap<long int, Intersection*>* intersections, GenericMap<long int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) ;
    virtual ~City();
	int getRoadMapSize();
	int getInstersectionMapSize();
	int getBoundsMapSize();
	GenericMap<long int, Link*>* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link, bool isIntersection);
    Route* randomPath(Intersection* startInt, Route* initialRoute, int totalLength, std::vector<float>* conditions);
	Intersection* getIntersection(long int intersectionNum);
    Route* getPath(Intersection* start, Intersection* end, std::vector<float>* conditions, int fastest);
    std::pair<std::vector<float>, std::vector<float>> getData(Road* road, int direction, SpeedPrediction* sp, Eigen::MatrixXd* spdIn, float dist);
    std::pair<std::vector<float>, std::vector<float>> routeToData(Route* route, float dist, SpeedPrediction* sp, Eigen::MatrixXd* spdIn);
	GenericMap<int, Bounds*>* getBoundsMap();
	GenericMap<long int, Road*>* getRoads();
	GenericMap<long int, Intersection*>* getIntersections();
    bool legalRoute(Route* route);
    void addRoad(Road* road);
    void addBounds(Bounds* bounds);
    void addIntersection(Intersection* intersection);
    Route* getRouteFromGPSTrace(GenericMap<long int, std::pair<double, double>*>* trace);
    void printIntersectionsAndRoads();
    bool roadIsOnTrace(Road* road, GenericMap<long int, std::pair<double, double>*>* trace);

};

} /* namespace PredictivePowertrain */

#endif /* CITY_CITY_H_ */
