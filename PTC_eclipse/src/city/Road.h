/*
 * Road.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROAD_H_
#define ROAD_H_

#include "../map/GenericMap.h"
#include <string>

#include "../data_management/Node.h"
#include "spline.h"

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	GenericMap<long int, Node*>* nodes;
	std::string roadType;
	Intersection* startNode;
	Intersection* endNode;
	tk::spline* spline;
	long int roadID;
	int boundsID;

public:
	Road();
	Road(std::string roadType, long int roadID, GenericMap<long int, Node*>* nodes);
	void copy(Road* other);
	virtual ~Road();
	void setStartIntersection(Intersection* startNode);
	void setEndIntersection(Intersection* endNode);
	Intersection* getStartIntersection();
	Intersection* getEndIntersection();
	int getRoadID();
	int* getElevData();
	int* getSpeedData();
	int getBoundsID();
	void setBoundsID(int id);
	std::string getRoadType();
	GenericMap<long int, Node*>* getNodes();
	tk::spline* getSpline();
	void assignSpline(tk::spline* spline);
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
