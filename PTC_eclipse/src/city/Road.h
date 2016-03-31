/*
 * Road.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROAD_H_
#define ROAD_H_

#include <string>

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	int* elevationData;
	int* speedLimitData;
	Intersection* startNode;
	Intersection* endNode;
//	RoadTypes roadType;
	int roadID;

public:
	Road();
	Road(std::string roadType, int* elevationData, int* speedData, int roadID);
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
