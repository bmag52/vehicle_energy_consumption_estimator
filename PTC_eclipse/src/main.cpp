/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <iostream>
#include <assert.h>

#include "data_collection/DataCollection.h"
#include "speed_prediction/SpeedPrediction.h"
#include "map/GenericMap.h"
#include "unit_tests/UnitTests.h"

using namespace PredictivePowertrain;

using Eigen::MatrixXd;

int main() {

	// unit test function calls here
	// TODO make a function that calls unit tests
	// link_UT();
	// route_ut();
	// goalToLinkMap_ut();


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

	// xml parser muck around
	DataCollection test;
//	test.getRoads();
	test.pullOSMData(47.681, -122.328);
	test.pullSRTMData(47.681, -122.328);
	std::cout << "finished" << std::endl;

	return 0;
}
