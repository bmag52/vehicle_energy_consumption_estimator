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
#include "city/City.h"
#include "city/Road.h"
#include "city/Intersection.h"
#include "map/GenericMap.h"
#include "unit_tests/UnitTests.h"

using namespace PredictivePowertrain;

using Eigen::MatrixXd;

int main() {

	// -------------------------- unit test function calls here --------------------------
	// TODO make a function that calls unit tests
//	link_UT();
//	route_ut();
//	linkToStateMap_ut();

	// -------------------------- speed_prediction muck around --------------------------
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

	// -------------------- data collection muck around --------------------------
//	DataCollection testDC;
//	testDC.pullData(47.681, -122.328); // greenlake
//	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
//	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;
//	testDC.pullData(47.618174, -122.330838); // downtown
//	std::cout << "node size " << testDC.getNodeMap()->getSize() << std::endl;
//	std::cout << "boundCount " << testDC.getBoundsMap()->getSize() << std::endl;
//	std::cout << "road Count " << testDC.makeRawRoads()->getSize() << std::endl;

	// -------------------------- data management muck around --------------------------
	DataManagement testDM;

	// test trip log add and get
	GenericMap<double, double>* latLon = new GenericMap<double, double>();
	latLon->addEntry(47.654, -122.345);
	latLon->addEntry(47.653, -122.346);
	latLon->addEntry(47.652, -122.347);
	testDM.addTripData(latLon, false);
	testDM.addTripData(latLon, true);
	GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* data = testDM.getMostRecentTripData();

	// test city logging add
	GenericMap<int, Node*>* nodes = new GenericMap<int, Node*>();
	nodes->addEntry(1, new Node(42.3, -122.4, 44, 1234566));
	nodes->addEntry(2, new Node(42.3, -122.4, 44, 1234566));

	GenericMap<int, Road*>* blankRoads;
	GenericMap<int, Bounds*>* boundsMap = new GenericMap<int, Bounds*>();
	boundsMap->addEntry(0, new Bounds(2, 3, 4, 5));
	boundsMap->addEntry(1, new Bounds(2, 3, 4, 5));
	boundsMap->addEntry(2, new Bounds(2, 3, 4, 5));

	Intersection* startInt = new Intersection(blankRoads, 42.3, -122.4, 44, 345);
	Intersection* endInt = new Intersection(blankRoads, 42.3, -122.4, 44, 456);

	Road* road1 = new Road("hilly", 987654, nodes);
	road1->setStartIntersection(startInt);
	road1->setEndIntersection(endInt);
	road1->setBoundsID(0);

	Road* road2 = new Road("flat", 8765, nodes);
	road2->setStartIntersection(startInt);
	road2->setEndIntersection(endInt);
	road2->setBoundsID(1);

	Road* road3 = new Road("medium", 9854, nodes);
	road3->setStartIntersection(startInt);
	road3->setEndIntersection(endInt);
	road3->setBoundsID(2);

	GenericMap<int, Road*>* roads = new GenericMap<int, Road*>();
	roads->addEntry(1, road1);
	roads->addEntry(2, road2);
	roads->addEntry(3, road3);

	Intersection* intersection1 = new Intersection(roads, 42.3, -122.4, 44, 345);
	intersection1->setBoundsID(0);
	Intersection* intersection2 = new Intersection(roads, 42.3, -122.4, 44, 345);
	intersection2->setBoundsID(1);
	Intersection* intersection3 = new Intersection(roads, 42.3, -122.4, 44, 345);
	intersection3->setBoundsID(2);

	GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
	intersections->addEntry(1, intersection1);
	intersections->addEntry(2, intersection2);
	intersections->addEntry(3, intersection3);

	City* city = new City(intersections, roads, boundsMap);
	testDM.addCityData(city);

	std::cout << "finished" << std::endl;

	return 0;
}
