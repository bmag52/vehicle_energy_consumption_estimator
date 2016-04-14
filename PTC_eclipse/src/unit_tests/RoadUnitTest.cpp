/*
 * RoadUnitTest.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: vagrant
 */

#include "../city/Road.h"
#include "../city/Intersection.h"
#include <assert.h>
#include "UnitTests.h"

using namespace std;

void road_ut() {

	int elevationData = 10, speedData = 15;
	int * test, * test2;

	test = &elevationData;
	test2 = &speedData;

	//Intersection::Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum) {
	Intersection sect = new intersection(1, 2.0, 2.0, 1, 1);

	//Testing default constructor
	Road road1;
	assert(road1.getRoadID() == 0);
	assert(road1.getSpeedData() == 0 && road1.getElevData() == 0);

	//Testing constructor Road::Road(std::string roadType, int* elevationData, int* speedData, int roadID)
	Road road2 = Road("default", &test, &test2, 1);

	//Testing setStartNode
	road2.setStartNode(&sect);

	//Testing setEndNode
	road2.setEndNode(&sect);

	//Testing getStartNode and getEndNode
	assert(road2.getStartNode() == sect.startNode && road2.getEndNode() == sect.endNode);

	//Testing getRoadID
	assert(road2.getRoadID() == 1);
	//Testing getSpeedData & get ElevData
	assert(road2.getSpeedData() == 15 && road2.getElevData() == 10);

}



