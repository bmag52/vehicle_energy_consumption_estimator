/*
 * Way.cpp
 *
 *  Created on: Apr 15, 2016
 *      Author: vagrant
 */

#include "Way.h"

namespace PredictivePowertrain {

Way::Way() {
}

Way::Way(GenericMap<int, long int>* nodeIDs, long int id, std::string wayType, int waySpeed) {
	this->nodeIDs = nodeIDs;
	this->id = id;
	this->wayType = wayType;
	this->waySpeed = waySpeed;
}

GenericMap<int, long int>* Way::getNodeIDs() {
	return this->nodeIDs;
}

long int Way::getID() {
	return this->id;
}

std::string Way::getWayType() {
	return this->wayType;
}

int Way::getWaySpeed() {
	return this->waySpeed;
}

}


