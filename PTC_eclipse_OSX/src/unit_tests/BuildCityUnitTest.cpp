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
}


