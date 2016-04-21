/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <iostream>
#include <assert.h>

#include "data_management/DataCollection.h"
#include "data_management/DataManagement.h"
#include "speed_prediction/SpeedPrediction.h"
#include "map/GenericMap.h"
#include "unit_tests/UnitTests.h"

using namespace PredictivePowertrain;

using Eigen::MatrixXd;

int main() {

	// unit test function calls here
	// TODO make a function that calls unit tests
//	link_UT();
//	route_ut();
//	linkToStateMap_ut();

	// speed_prediction muck around
	SpeedPrediction sp;

	int I = sp.getI();
	int O = sp.getO();

	Eigen::MatrixXd spd_in = Eigen::MatrixXd::Random(1,I+1)*150;
	Eigen::MatrixXd spd_act = Eigen::MatrixXd::Random(1,O);
	Eigen::MatrixXd spd_pred = Eigen::MatrixXd::Zero(1,O);

	std::cout << spd_in << std::endl;
	sp.formatInData(&spd_in);
	std::cout << spd_in << std::endl;

	sp.predict(&spd_in, &spd_pred);
	sp.train(&spd_pred, &spd_act, &spd_in);

	std::cout << spd_pred << std::endl;
	sp.formatOutData(&spd_pred);
	std::cout << spd_pred << std::endl;

	// route_prediction muck around
	// GenericMap<int, int> test;

	// data collection muck around
//	DataCollection testDC;
//	testDC.pullData(47.681, -122.328); // greenlake
//	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
//	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;
//	testDC.pullData(47.618174, -122.330838); // downtown
//	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
//	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;
//	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;

	// data management muck around
	DataManagement testDM;
	GenericMap<double, double>* latLon = new GenericMap<double, double>();
	latLon->addEntry(47.654, -122.345);
	latLon->addEntry(47.653, -122.346);
	latLon->addEntry(47.652, -122.347);
	testDM.addTripData(latLon, false);
	testDM.addTripData(latLon, true);
	GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* data = testDM.getMostRecentTripData();

	std::cout << "finished" << std::endl;

	return 0;
}
