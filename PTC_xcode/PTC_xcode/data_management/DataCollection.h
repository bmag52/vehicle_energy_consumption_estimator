/*
 * DataCollection.h
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
#include "../gps/GPS.h"
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
	std::string dataFolder = "/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data";
	std::string mapPNGName =  "mapImage.png";
	int** eleDataSRTM;
	int numEleLatsSRTM;
	int numEleLonsSRTM;
	int wayCount;
	int boundsCountXML;
	int boundsCountPNG;
    int zoomMax;
    int zoomMin;
	double eleLowerLeftLat;
	double eleLowerLeftLon;
	double eleCellSize;
	int voidEle;
	int maxVisEntries = 5000;
	GenericMap<long int, Node*> nodeMap;
	GenericMap<int, Way*> wayMap;
	GenericMap<int, Bounds*> boundsMapXML;
	GenericMap<int, Bounds*> boundsMapPNG;
	GenericMap<int, std::pair<double, double>*> zoomSpreads;

	const boost::property_tree::ptree& empty_ptree();
	void queryFile(std::string serverName, std::string getCommand, std::string fileName);
	std::string getBin(double hi, double lo, int bins, double latLon, bool isLat);
	void checkDataFoler();
	void pullSRTMData(double lat, double lon);
	void pullOSMDataXML(double lat, double lon);
	int pullOSMDataPNG(double lat, double lon);
	void setZoomSpreads();
    void initialize(double latDelta, double lonDelta);

public:
	DataCollection();
	DataCollection(double latDelta, double lonDelta);
	void pullDataXML(double lat, double lon);
	int pullDataPNG(double lat, double lon);
	GenericMap<long int, Node*>* getNodeMap();
	GenericMap<int, Way*>* getWayMap();
	GenericMap<int, Bounds*>* getBoundsMapXML();
	GenericMap<int, Bounds*>* getBoundsMapPNG();
	int getVoidEle();
	GenericMap<long int, Road*>* makeRawRoads();
	std::string getDataFolder();
	std::string getMapPNGName();
	int getElevation(double lat, double lon);

};



} /* namespace PredictivePowertrain */

#endif /* DATACOLLECTION_MAKEOSM_H_ */
