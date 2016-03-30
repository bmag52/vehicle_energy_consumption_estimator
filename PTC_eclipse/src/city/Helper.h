/*
 * Helper.h
 *
 *  Created on: Mar 28, 2016
 *      Author: vagrant
 */

#ifndef CITY_HELPER_H_
#define CITY_HELPER_H_

#include "Intersection.h"
#include "Road.h"
#include "../driver_prediction/Link.h"

namespace PredictivePowertrain {

class Intersection;
Link* linkFromRoad(Road* road, Intersection* intersection);

}

#endif /* CITY_HELPER_H_ */
