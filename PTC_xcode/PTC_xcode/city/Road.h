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
#include "../gps/GPS.h"

#include <string>
#include <limits.h>
#include <utility>
#include <vector>
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
    double maxLat;
    double minLat;
    double maxLon;
    double minLon;
    void initialize();

public:
	Road();
	Road(std::string roadType, long int roadID, GenericMap<long int, Node*>* nodes);
	void copy(Road* other);
	virtual ~Road();
    void assignID(long int id);
	void setStartIntersection(Intersection* startNode);
	void setEndIntersection(Intersection* endNode);
	Intersection* getStartIntersection();
	Intersection* getEndIntersection();
	long int getRoadID();
    void getElevData(std::vector<float>* elev, std::vector<float>* dist);
    std::vector<float>* getSpeedData();
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
    void setMinMaxLatLon();
    double getMaxLat();
    double getMinLat();
    double getMaxLon();
    double getMinLon();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
