/*
 * Road.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "Road.h"

namespace PredictivePowertrain {

Road::Road() {
}

Road::Road(std::string roadType, int* elevationData, int* speedData, int roadID) {
	this->elevationData = elevationData;
	this->speedLimitData = speedData;
//	this-> = new RoadTypes(roadType);
	this->roadID = roadID;
}

void Road::setStartNode(Intersection* startNode) {
	this->startNode = startNode;
}

void Road::setEndNode(Intersection* endNode) {
	this->endNode = endNode;
}

Intersection* Road::getStartNode() {
	return this->startNode;
}

Intersection* Road::getEndNode() {
	return this->endNode;
}

int Road::getRoadID() {
	return this->roadID;
}

Road::~Road() {
}

int* Road::getSpeedData() {
	return this->speedLimitData;
}

int* Road::getElevData() {
	return this->elevationData;
}

} /* namespace PredictivePowertrain */
