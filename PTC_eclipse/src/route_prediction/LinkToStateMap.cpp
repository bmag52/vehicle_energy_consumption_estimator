/*
 * LinkToStateMap.cpp
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#include "LinkToStateMap.h"
#include "GenericMap.h"

namespace RoutePrediction {

int LinkToStateMap::incrementTransition(Link link, Goal goal, Link link1) {
}

GenericEntry<double, double> LinkToStateMap::getProbability(Link link, Link link1, Goal goal, bool bool1) {
}

LinkToStateMap::LinkToStateMap(){
}

LinkToStateMap::LinkToStateMap(LinkToStateMap& other) {
}

LinkToStateMap::~LinkToStateMap() {
}

} /* namespace RoutePrediction */
