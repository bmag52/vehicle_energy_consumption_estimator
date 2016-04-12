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

// xml parse
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"

// query
#include "boost/lexical_cast.hpp"
#include "boost/asio.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
//#include <unistd.h>
//#include <sys/stat.h>

namespace PredictivePowertrain {

class MakeOSM {
private:
	double latDelta;
	double lonDelta;
	std::string xmlFile = "./test.xml";

public:
	MakeOSM();
	MakeOSM(double latDelta, double lonDelta);
	void pullOSMData(double lat, double lon);
	Road* getRoads();
	Intersection* getIntersections(Road* roads);
	const boost::property_tree::ptree& empty_ptree();
};

} /* namespace PredictivePowertrain */

#endif /* OSM_MAKEOSM_H_ */
