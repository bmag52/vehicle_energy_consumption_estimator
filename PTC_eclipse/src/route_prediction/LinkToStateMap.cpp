/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"
#include "GenericMap.h"

namespace RoutePrediction {

int LinkToStateMap::incrementTransition(Link* link1, Goal* goal, Link* link2) {
	// TODO incrememntTransistion
}

double LinkToStateMap::getProbability(Link* &link1, Link* &link2, Goal& goal, bool isSimilar) {
	// TODO getProbability
}

LinkToStateMap::LinkToStateMap(){
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other) {
}

LinkToStateMap::~LinkToStateMap() {
}

} /* namespace RoutePrediction */


