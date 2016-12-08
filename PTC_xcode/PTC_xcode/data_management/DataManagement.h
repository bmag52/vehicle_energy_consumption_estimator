/*
 * DataManagement.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#ifndef DATA_MANAGEMENT_DATAMANAGEMENT_H_
#define DATA_MANAGEMENT_DATAMANAGEMENT_H_

#include "../route_prediction/RoutePrediction.h"
#include "../city/Intersection.h"
#include "../city/Road.h"
#include "../city/City.h"
#include "../map/GenericMap.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "boost/lexical_cast.hpp"

#include <eigen3/Eigen/Dense>

#include <tuple>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <list>

namespace PredictivePowertrain {

class DataManagement {
private:
    std::string saveFolder = "/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/";
	std::string routePredictionData = saveFolder + "routePrediction.json";
	std::string cityData = saveFolder + "cities.json";
	std::string tripData = saveFolder + "tripLogs.json";
    std::string nnDataLoc = saveFolder + "nnData.bin";
    
    bool jsonifyLinkNNData = false;
    
    int countFileLine(std::string fileLoc);
    
public:
	DataManagement();
	void addRoutePredictionData(RoutePrediction* rp);
	void addCityData(City* city);
	void addTripData(GenericMap<long int, std::pair<double, double>*>* latLon);
	RoutePrediction* getRoutePredictionData();
	City* getCityData();
	GenericMap<long int, std::pair<double, double>*>* getMostRecentTripData();
    
    // for serializing matrices
    template<class Matrix> void write_binary(std::ofstream& out, const Matrix& matrix, int matrixType, int layerNum, long int linkHash);
    template<class Matrix> void read_binary(std::ifstream& in, Matrix& matrix, int& matrixTypeNum, int& layerNum, long int& linkHash);

};

} /* namespace PredictivePowertrain */


#endif /* DATA_MANAGEMENT_DATAMANAGEMENT_H_ */
