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
//	Road* roads;
	GenericMap<long int, Road*>* roads;
//	IntersectionTypes interSectionType;
	int id;
	double elevation;
	double lat;
	double lon;
	int boundsID;
	Link* link;

public:
	Intersection();
	Intersection(GenericMap<long int, Road*>* roads, double lat, double lon, int elev, int intersectNum);
	void setBoundsID(int id);
	virtual ~Intersection();
	void addRoad(Road* road, int roadDir);
	GenericMap<int, Link*>* getOutgoingLinks();
	int getRoadCount();
	int getIntersectionID();
	double getElevation();
	double getLat();
	double getLon();
	GenericMap<long int, Road*>* getRoads();
	GenericMap<int, Intersection*>* getAdjacentIntersection();
	Intersection* getNextIntersection(Road* road);
	int getBoudsID();
};

} /* namespace PredictivePowertrain */

#endif /* INTERSECTION_H_ */
