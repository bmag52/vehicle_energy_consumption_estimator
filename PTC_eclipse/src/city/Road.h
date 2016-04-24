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

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	GenericMap<int, Node*>* nodes;
	std::string roadType;
	Intersection* startNode;
	Intersection* endNode;
	long int roadID;
	int boundsID;

public:
	Road();
	Road(std::string roadType, long int roadID, GenericMap<int, Node*>* nodes);
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
	GenericMap<int, Node*>* getNodes();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
