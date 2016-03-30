/*
 * Helper.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: vagrant
 */
#include "Helper.h"

namespace PredictivePowertrain {

Link* linkFromRoad(Road* road, Intersection* intersection) {
	int linkNum = road->getRoadID();
	int linkDir = road->getEndNumber();
	Link newLink(linkDir, linkNum);
	return &newLink;
}

}

