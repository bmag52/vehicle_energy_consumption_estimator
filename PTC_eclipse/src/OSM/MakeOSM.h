/*
 * MakeOSM.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef OSM_MAKEOSM_H_
#define OSM_MAKEOSM_H_

#include "../city/Road.h"
#include "../city/Intersection.h"

#include "pugixml-1.7/src/pugixml.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/asio.hpp"

#include <fstream>
#include <string>

namespace PredictivePowertrain {

class MakeOSM {
private:
	double latDelta;
	double lonDelta;

public:
	MakeOSM();
	MakeOSM(double latDelta, double lonDelta);
	void pullOSMData(double lat, double lon);
	Road* getRoads();
	Intersection* getIntersections(Road* roads);
};

} /* namespace PredictivePowertrain */

#endif /* OSM_MAKEOSM_H_ */
