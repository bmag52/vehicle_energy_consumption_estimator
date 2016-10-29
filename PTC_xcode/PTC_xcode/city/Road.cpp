/*
 * Road.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "Road.h"

namespace PredictivePowertrain {

Road::Road() {
	this->roadID = 0;
    this->initialize();
}

Road::Road(std::string roadType, long int roadID, GenericMap<long int, Node*>* nodes) {
	this->roadType = roadType;
	this->roadID = roadID;
	this->nodes = nodes;
    this->initialize();
}
    
void Road::initialize()
{
    this->boundsID = 0;
    this->startNode = NULL;
    this->endNode = NULL;
    this->splineLength = -1;
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
    
double Road::getMaxLat()
{
    return this->maxLat;
}

double Road::getMinLat()
{
    return this->minLat;
}

double Road::getMaxLon()
{
    return this->maxLon;
}

double Road::getMinLon()
{
    return this->minLon;
}

std::vector<float>* Road::getSpeedData() {
    // TODO
    return NULL;
}

void Road::getElevData(std::vector<float>* elev, std::vector<float>* dist)
{
    GPS converter;
    
    this->nodes->initializeCounter();
    GenericEntry<long int, Node*>* prevNode = this->nodes->nextEntry();
    GenericEntry<long int, Node*>* nextNode = this->nodes->nextEntry();
    
    while(nextNode != NULL)
    {
        elev->push_back(nextNode->value->getEle());
        dist->push_back(converter.deltaLatLonToXY(prevNode->value->getLat(), prevNode->value->getLon(), nextNode->value->getLat(), nextNode->value->getLon()));
        
        prevNode = nextNode;
        nextNode = this->nodes->nextEntry();
    }
    delete(nextNode);
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
    this->setMinMaxLatLon();
    return new std::pair<double, double>(this->maxLat - this->minLat, this->maxLon - this->minLon);
}
    
void Road::setMinMaxLatLon()
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
            this->minLat = lat;
        }
        
        if(lat > maxLat)
        {
            this->maxLat = lat;
        }
        
        if(lon < minLon)
        {
            this->minLon = lon;
        }
        
        if(lon > maxLon)
        {
            this->maxLon = lon;
        }
    }
}

} /* namespace PredictivePowertrain */


