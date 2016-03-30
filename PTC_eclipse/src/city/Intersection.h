/*
 * Intersection.h
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#ifndef CITY_INTERSECTION_H_
#define CITY_INTERSECTION_H_

#include "IntersectionTypes.h"
#include "Road.h"
#include "../driver_prediction/Link.h"

namespace PredictivePowertrain {

class Intersection {
private:
	Road* roads; //%array of roads that connect to this intersection
//	IntersectionTypes interSectionType;
	int roadCount;
	int number;
	double elevation;
	double lat;
	double lon;

public:
	Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum);
	Intersection();
	virtual ~Intersection();
	void addRoad(Road* road, int roadDir);
	Link* getOutgoingLinks();
	int getNumber();
	double getElevation();
	double getLat();
	double getLon();
	Road* getRoads();
	int* getAdjacentIntersectionNumbers();
	int getNextIntersectionNumber(Road* road);
};

} /* namespace PredictivePowertrain */

#endif /* CITY_INTERSECTION_H_ */
