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

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>

namespace PredictivePowertrain {

class MakeOSM {
private:
	double latDelta;
	double lonDelta;
	std::string mapFile = "./map.xml";
	std::string eleFile = "./eleFile.aci";
	std::string testXml = "./test.xml";
	int** eleData;
	int numEleCols;
	int numEleRows;
	int eleLowerLeftLat;
	int eleLowerLeftLon;
	double eleCellSize;
	int voidEle;


	const boost::property_tree::ptree& empty_ptree();
	void queryFile(std::string serverName, std::string getCommand, std::string fileName);
	std::string getBin(double hi, double lo, int bins, double latLon, bool isLat);
public:
	MakeOSM();
	MakeOSM(double latDelta, double lonDelta);
	void pullSRTMData(double lat, double lon);
	void pullOSMData(double lat, double lon);
	Road* getRoads();
	Intersection* getIntersections(Road* roads);

};

} /* namespace PredictivePowertrain */

#endif /* OSM_MAKEOSM_H_ */
