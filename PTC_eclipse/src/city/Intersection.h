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

using namespace DriverPrediction;

namespace City {

class Intersection {
public:
	Road* roads; //%array of roads that connect to this intersection
//	IntersectionTypes interSectionType;
	int elevation;
	int number;
	double lat;
	double lon;
	Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum);
	Intersection();
	virtual ~Intersection();
	void getOutgoingLinks(Link * link);
};

} /* namespace City */

#endif /* CITY_INTERSECTION_H_ */
