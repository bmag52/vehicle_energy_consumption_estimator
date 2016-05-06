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

int Road::getRoadID() {
	return this->roadID;
}

Road::~Road() {
}

int* Road::getSpeedData() {
}

int* Road::getElevData() {
}

int Road::getBoundsID() {
	return this->boundsID;
}

void Road::setBoundsID(int id) {
	this->boundsID = id;
}

std::string Road::getRoadType() {
	return this->roadType;
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

void Road::assignAdjMatIndicies(GenericMap<int, int>* adjMatIndicies) {
	this->adjMatIndices = adjMatIndicies;
}

GenericMap<int, int>* Road::getAdjMatIndicies() {
	return this->adjMatIndices;
}

} /* namespace PredictivePowertrain */


