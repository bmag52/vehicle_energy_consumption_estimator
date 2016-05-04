/*
 * MakeOSM.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef DATACOLLECTION_MAKEOSM_H_
#define DATACOLLECTION_MAKEOSM_H_

#define EIGEN_NO_DEBUG

#include "../city/Road.h"
#include "../city/Intersection.h"
#include "../map/GenericMap.h"
#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"

// query
#include "boost/lexical_cast.hpp"
#include "boost/asio.hpp"

#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include "../data_management/Bounds.h"
#include "../data_management/Node.h"
#include "../data_management/Way.h"

namespace PredictivePowertrain {

class DataCollection {
private:
	double latDelta;
	double lonDelta;
	std::string mapFile;
	std::string eleFile;
	std::string dataFolder = "data";
	int** eleData;
	int numEleLats;
	int numEleLons;
	int wayCount;
	int boundsCount;
	double eleLowerLeftLat;
	double eleLowerLeftLon;
	double eleCellSize;
	int voidEle;
	int maxVisEntries = 5000;
	GenericMap<long int, Node*> nodeMap;
	GenericMap<int, Way*> wayMap;
	GenericMap<int, Bounds*> boundsMap;

	const boost::property_tree::ptree& empty_ptree();
	void queryFile(std::string serverName, std::string getCommand, std::string fileName);
	std::string getBin(double hi, double lo, int bins, double latLon, bool isLat);
	void checkDataFoler();
	void pullSRTMData(double lat, double lon);
	void pullOSMData(double lat, double lon);
	int getElevation(double lat, double lon);

public:
	DataCollection();
	DataCollection(double latDelta, double lonDelta);
	void pullData(double lat, double lon);
	GenericMap<long int, Node*>* getNodeMap();
	GenericMap<int, Way*>* getWayMap();
	GenericMap<int, Bounds*>* getBoundsMap();
	int getVoidEle();
	GenericMap<long int, Road*>* makeRawRoads();
};

} /* namespace PredictivePowertrain */

#endif /* DATACOLLECTION_MAKEOSM_H_ */
