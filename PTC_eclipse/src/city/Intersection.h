/*
 * Intersection.h
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include "IntersectionTypes.h"
#include "Road.h"
#include "../driver_prediction/Link.h"

namespace PredictivePowertrain {

class Link; // forward declaration

class Intersection {
private:
	Road* roads; //%array of roads that connect to this intersection
//	IntersectionTypes interSectionType;
	int roadCount;
	int id;
	double elevation;
	double lat;
	double lon;
	int boundsID;
	Link* link;

public:
	Intersection();
	Intersection(Road* roadInput, double lat, double lon, int elev, int intersectNum);
	void setBoundsID(int id);
	virtual ~Intersection();
	void addRoad(Road* road, int roadDir);
	Link* getOutgoingLinks();
	int getRoadCount();
	int getIntersectionID();
	double getElevation();
	double getLat();
	double getLon();
	Road* getRoads();
	Intersection* getAdjacentIntersection();
	Intersection* getNextIntersection(Road* road);
	int getBoudsID();
};

} /* namespace PredictivePowertrain */

#endif /* INTERSECTION_H_ */
