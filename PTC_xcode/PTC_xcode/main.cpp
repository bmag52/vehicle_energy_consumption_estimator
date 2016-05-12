/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <iostream>
#include <assert.h>

#include "unit_tests/UnitTests.h"
#include "map/GenericMap.h"
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "boost/foreach.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/asio.hpp"

#include "map/GenericMap.h"
#include "data_management/DataManagement.h"
#include "city/BuildCity.h"

using namespace PredictivePowertrain;
using namespace boost::asio;
using boost::asio::ip::tcp;
using boost::lexical_cast;

int main() {

//	dataManagement_ut();
//	dataCollection_ut();
//	buildCity_ut();
    
        
    // make trip log
    GenericMap<long int, std::pair<double, double>*>* latLon = new GenericMap<long int, std::pair<double, double>*>();
    latLon->addEntry(1, new std::pair<double, double>(47.681, -122.328));
    latLon->addEntry(2, new std::pair<double, double>(47.682, -122.327));
    latLon->addEntry(3, new std::pair<double, double>(47.683, -122.326));
    latLon->addEntry(4, new std::pair<double, double>(47.684, -122.325));
    latLon->addEntry(5, new std::pair<double, double>(47.685, -122.324));
    latLon->addEntry(6, new std::pair<double, double>(47.686, -122.323));
    latLon->addEntry(7, new std::pair<double, double>(47.687, -122.322));
    
    // jsonify trip log -> delete existing jsons
    DataManagement dm;
    dm.addTripData(latLon);
    
    BuildCity bc;
    bc.updateGridDataPNG();
    
    
    Eigen::MatrixXd testEigen;
    
    cv::Mat testOpenCv;
    
    // boost test
    std::cout << lexical_cast<double>("2") << std::endl;

	std::cout << "finished" << std::endl;

	return 0;
}
