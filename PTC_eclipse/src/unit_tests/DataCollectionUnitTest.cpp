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
	testDC.pullData(47.681, -122.328); // greenlake
	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;

	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;

	testDC.pullData(47.618174, -122.330838); // downtown
	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;

	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;

}
