/*
 * BuildCityUnitTest.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../map/GenericMap.h"
#include "../data_management/DataManagement.h"
#include "../city/BuildCity.h"

using namespace PredictivePowertrain;

void buildCity_ut() {
    
    // make trip log
    GenericMap<long int, std::pair<double, double>*>* latLon = new GenericMap<long int, std::pair<double, double>*>();
    
    // greenlake trip log small
    latLon->addEntry(1, new std::pair<double, double>(47.634, -117.396));
    latLon->addEntry(2, new std::pair<double, double>(47.635, -117.397));
    latLon->addEntry(3, new std::pair<double, double>(47.636, -117.398));
    latLon->addEntry(4, new std::pair<double, double>(47.637, -117.399));
    latLon->addEntry(5, new std::pair<double, double>(47.638, -117.400));
    latLon->addEntry(6, new std::pair<double, double>(47.639, -117.401));
    latLon->addEntry(7, new std::pair<double, double>(47.640, -117.402));
    
    // down town trip log big
//    latLon->addEntry(1, new std::pair<double, double>(47.656784, -122.307302));
//    latLon->addEntry(2, new std::pair<double, double>(47.560249, -122.379874));

    // jsonify trip log -> delete existing jsons
    DataManagement dm;
    dm.addTripData(latLon);
    
    BuildCity bc;
//    bc.updateGridDataPNG();
    bc.updateGridDataXMLSpline();
    bc.printNewIntersectionsAndRoads();
}
