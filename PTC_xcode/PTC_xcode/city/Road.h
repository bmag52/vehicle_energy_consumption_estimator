/*
 * Road.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROAD_H_
#define ROAD_H_

#include "../map/GenericMap.h"
#include "../data_management/Node.h"

#include <string>
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	Eigen::Spline<float, 2> spline;
	GenericMap<long int, Node*>* nodes;
	GenericMap<int, std::pair<int, int>*>* adjMatIndices;
	std::string roadType;
	Intersection* startNode;
	Intersection* endNode;
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
	long int getRoadID();
	int* getElevData();
	int* getSpeedData();
	int getBoundsID();
	void setBoundsID(int id);
	std::string getRoadType();
	GenericMap<long int, Node*>* getNodes();
	Eigen::Spline<float, 2> getSpline();
	void assignSpline(Eigen::Spline<float, 2> spline);
	void assignAdjMatIndicies(GenericMap<int, std::pair<int, int>*>* adjMatIndicies);
	GenericMap<int, std::pair<int, int>*>* getAdjMatIndicies();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
