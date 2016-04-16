/*
 * MakeOSM.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef DATACOLLECTION_MAKEOSM_H_
#define DATACOLLECTION_MAKEOSM_H_

#include "../city/Road.h"
#include "../city/Intersection.h"
#include "../map/GenericMap.h"
#include "Node.h"
#include "Way.h"

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
#include <sys/stat.h>

namespace PredictivePowertrain {

class DataCollection {
private:
	double latDelta;
	double lonDelta;
	std::string mapFile;
	std::string eleFile;
	std::string dataFolder = "data";
	std::string testXml = "./test.xml";
	int** eleData;
	int numEleCols;
	int numEleRows;
	double eleLowerLeftLat;
	double eleLowerLeftLon;
	double eleCellSize;
	int voidEle;
	GenericMap<long int, Node*> nodeMap;
	GenericMap<long int, Way*> wayMap;


	const boost::property_tree::ptree& empty_ptree();
	void queryFile(std::string serverName, std::string getCommand, std::string fileName);
	std::string getBin(double hi, double lo, int bins, double latLon, bool isLat);
	void checkDataFoler();
public:
	DataCollection();
	DataCollection(double latDelta, double lonDelta);
	void pullSRTMData(double lat, double lon);
	void pullOSMData(double lat, double lon);
	GenericMap<long int, Node*>* getNodeMap();
	GenericMap<long int, Way*>* getWayMap();
	int** getEleData();

};

} /* namespace PredictivePowertrain */

#endif /* DATACOLLECTION_MAKEOSM_H_ */
