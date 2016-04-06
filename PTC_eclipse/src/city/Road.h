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

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	int* elevationData;
	int* speedLimitData;
	double* lat;
	double* lon;
	GenericMap<int, int> nodes;
	Intersection* startNode;
	Intersection* endNode;
//	RoadTypes roadType;
	int roadID;

public:
	Road();
	Road(std::string roadType, int* elevationData, int* speedData, int roadID, double* lat, double* lon);
	virtual ~Road();
	void setStartNode(Intersection* startNode);
	void setEndNode(Intersection* endNode);
	Intersection* getStartNode();
	Intersection* getEndNode();
	int* getSpeedData();
	int* getElevData();
	int getRoadID();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
