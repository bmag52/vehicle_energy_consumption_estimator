/*
 * Helper.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: vagrant
 */

#include "../driver_prediction/Link.h"
#include "Road.h"
#include "Helper.h"


namespace PredictivePowertrain {

Link* linkFromRoad(Road* road, Intersection* intersection) {
	int linkNum = road->getRoadID();
//	road-> = intersection->getNumber();
//	int linkDir = road->endNode;
//	Link newLink(linkDir, linkNum);
//	return &newLink;
}

}

