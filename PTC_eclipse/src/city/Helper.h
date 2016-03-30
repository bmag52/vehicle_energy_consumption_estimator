/*
 * Helper.h
 *
 *  Created on: Mar 28, 2016
 *      Author: vagrant
 */

#ifndef CITY_HELPER_H_
#define CITY_HELPER_H_

#include "Road.h"
#include "Intersection.h"
#include "../driver_prediction/Link.h"



namespace PredictivePowertrain {

Link* linkFromRoad(Road* road, Intersection* intersection);

}

#endif /* CITY_HELPER_H_ */
