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
#include <string>

namespace PredictivePowertrain {

class Link; // forward declaration

class Intersection {
private:
//	Road* roads;
	GenericMap<long int, Road*>* roads;
//	IntersectionTypes interSectionType;
	long int ID;
	double elevation;
	double lat;
	double lon;
	int boundsID;
	Link* link;

public:
	Intersection();
	Intersection(GenericMap<long int, Road*>* roads, double lat, double lon, int elev, long int intersectNum);
	void setBoundsID(int ID);
	virtual ~Intersection();
	void addRoad(Road* road, int roadDir);
	GenericMap<int, Link*>* getOutgoingLinks();
	int getRoadCount();
	long int getIntersectionID();
	double getElevation();
	double getLat();
	double getLon();
	GenericMap<long int, Road*>* getRoads();
	GenericMap<int, Intersection*>* getAdjacentIntersection();
	Intersection* getNextIntersection(Road* road);
	int getBoudsID();
    void assignID(long int ID);
};

} /* namespace PredictivePowertrain */

#endif /* INTERSECTION_H_ */
