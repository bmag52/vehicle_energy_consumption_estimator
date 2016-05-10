/*
 * DataCollectionUnitTest.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../data_management/DataCollection.h"

using namespace PredictivePowertrain;

void dataCollection_ut() {

	DataCollection testDC;

	std::cout << testDC.pullDataPNG(47.681, -122.328) << std::endl; // greenlake

	testDC.pullDataXML(47.681, -122.328); // greenlake
	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
	std::cout << "boundCount " << testDC.getBoundsMapXML()->getSize() << std::endl;

	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;
	GenericMap<long int, Road*>* roads = testDC.makeRawRoads();

	testDC.pullDataXML(47.618174, -122.330838); // downtown
	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
	std::cout << "boundCount " << testDC.getBoundsMapXML()->getSize() << std::endl;
	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;
}
