/*
 * Road.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "Road.h"

namespace City {

Road::Road() {
}

Road::Road(std::string roadType, int* elevationData, int* speedData, int roadID) {
	this->elevationData = elevationData;
	this->speedLimitData = speedData;
//	this-> = new RoadTypes(roadType);
	this->roadID = roadID;
	this->startNumber = 0;
	this->endNumber = 0;
}

void Road::setStartNumber(int intersectionNumber) {
	this->startNumber = intersectionNumber;
}

int Road::getStartNumber() {
	return this->startNumber;
}

void Road::setEndNumber(int intersectionNumber) {
	this->endNumber = intersectionNumber;
}

int Road::getEndNumber() {
	return this->endNumber;
}

int Road::getRoadID() {
	return this->roadID;
}

Road::~Road() {
}

} /* namespace City */


