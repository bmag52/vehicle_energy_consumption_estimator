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
class Road; // forward declaration

class Intersection {
    
private:
//	Road* roads;
	GenericMap<long int, Road*>* roads;
//	IntersectionTypes interSectionType;
	long int ID;
	float elevation;
	double lat;
	double lon;
	int boundsID;
	Link* link;

public:
	Intersection();
	Intersection(GenericMap<long int, Road*>* roads, double lat, double lon, float elev, long int intersectNum);
	void setBoundsID(int ID);
	virtual ~Intersection();
	void addRoad(Road* road, int roadDir);
	GenericMap<long int, Link*>* getOutgoingLinks();
    GenericMap<long int, Link*>* getOutgoingLinks(Link* excluded);
	int getRoadCount();
	long int getIntersectionID();
	float getElevation();
	double getLat();
	double getLon();
	GenericMap<long int, Road*>* getRoads();
	GenericMap<long int, Intersection*>* getAdjacentIntersections();
	Intersection* getNextIntersection(Road* road);
	int getBoudsID();
    void assignID(long int ID);
    void replaceRoads(GenericMap<long int, Road*>* roads);
    void setElev(float newElev);
};

} /* namespace PredictivePowertrain */

#endif /* INTERSECTION_H_ */
