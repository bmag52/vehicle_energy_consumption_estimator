/*
 * Road.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "Road.h"

namespace PredictivePowertrain {

Road::Road() {
//	this-> = new RoadTypes("default");
	this->roadID = 0;
}

Road::Road(std::string roadType, long int roadID, GenericMap<long int, Node*>* nodes) {
	this->roadType = roadType;
	this->roadID = roadID;
	this->nodes = nodes;
}

void Road::setStartIntersection(Intersection* startNode) {
	this->startNode = startNode;
}

void Road::setEndIntersection(Intersection* endNode) {
	this->endNode = endNode;
}

Intersection* Road::getStartIntersection() {
	return this->startNode;
}

Intersection* Road::getEndIntersection() {
	return this->endNode;
}
void Road::assignID(long int id)
{
    this->roadID = id;
}

long int Road::getRoadID() {
	return this->roadID;
}

Road::~Road() {
}

std::vector<float>* Road::getSpeedData() {
    // TODO
    return NULL;
}

std::vector<float>* Road::getElevData() {
    // TODO
    return NULL;
}

int Road::getBoundsID() {
	return this->boundsID;
}

void Road::setBoundsID(int id) {
	this->boundsID = id;
}
    
float Road::getSplineLength()
{
    return this->splineLength;
}

std::string Road::getRoadType() {
	return this->roadType;
}
    
void Road::assignSplineLength(float dist)
{
    this->splineLength = dist;
}

GenericMap<long int, Node*>* Road::getNodes() {
	return this->nodes;
}

Eigen::Spline<double, 2> Road::getSpline() {
	return this->spline;
}

void Road::assignSpline(Eigen::Spline<double, 2> spline) {
	this->spline = spline;
}

void Road::assignAdjMatIndicies(GenericMap<int, std::pair<int, int>*>* adjMatIndicies) {
	this->adjMatIndices = adjMatIndicies;
}

GenericMap<int, std::pair<int, int>*>* Road::getAdjMatIndicies() {
	return this->adjMatIndices;
}

std::pair<double, double>* Road::getMidLatLon()
{
    double DBL_MAX = std::numeric_limits<double>::max();
    
    double minLat = DBL_MAX;
    double maxLat = -DBL_MAX;
    double minLon = DBL_MAX;
    double maxLon = -DBL_MAX;
    
    for(double u = 0; u <= 1; u += .025)
    {
        Eigen::Spline<double,2>::PointType point = this->spline(u);
        
        double lat = point(0,0);
        double lon = point(1,0);
        
        if(lat < minLat)
        {
            minLat = lat;
        }
        
        if(lat > maxLat)
        {
            maxLat = lat;
        }
        
        if(lon < minLon)
        {
            minLon = lon;
        }
        
        if(lon > maxLon)
        {
            maxLon = lon;
        }
    }
    
    return new std::pair<double, double>(maxLat - minLat, maxLon - minLon);
}

} /* namespace PredictivePowertrain */


