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
#include "../data_collection/Node.h"

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	GenericMap<int, Node*>* nodes;
	std::string roadType;
	Intersection* startNode;
	Intersection* endNode;
	int roadID;

public:
	Road();
	Road(std::string roadType, int roadID);
	virtual ~Road();
	void setStartNode(Intersection* startNode);
	void setEndNode(Intersection* endNode);
	Intersection* getStartNode();
	Intersection* getEndNode();
	int getRoadID();
	int* getElevData();
	int* getSpeedData();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
