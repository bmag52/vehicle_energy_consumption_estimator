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
#include <limits.h>
#include <utility>
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Road {
private:
	Eigen::Spline<double, 2> spline;
    float splineLength;
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
    float getSplineLength();
	std::string getRoadType();
	GenericMap<long int, Node*>* getNodes();
	Eigen::Spline<double, 2> getSpline();
	void assignSpline(Eigen::Spline<double, 2> spline);
    void assignSplineLength(float dist);
	void assignAdjMatIndicies(GenericMap<int, std::pair<int, int>*>* adjMatIndicies);
	GenericMap<int, std::pair<int, int>*>* getAdjMatIndicies();
    std::pair<double, double>* getMidLatLon();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
