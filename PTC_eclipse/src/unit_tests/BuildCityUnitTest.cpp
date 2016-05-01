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
	latLon->addEntry(1, new std::pair<double, double>(0,0));
	latLon->addEntry(2, new std::pair<double, double>(1,1));
	latLon->addEntry(3, new std::pair<double, double>(2,2));

	// jsonify trip log -> delete existing jsons
	DataManagement dm;
	dm.addTripData(latLon);

	BuildCity bc;
	bc.updateGridData();

}


