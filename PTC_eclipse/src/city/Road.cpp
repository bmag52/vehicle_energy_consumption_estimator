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

Road::Road(std::string roadType, long int roadID, GenericMap<int, Node*>* nodes) {
	this->roadType = roadType;
	this->roadID = roadID;
	this->nodes = nodes;
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
}

int* Road::getElevData() {
}

} /* namespace PredictivePowertrain */
