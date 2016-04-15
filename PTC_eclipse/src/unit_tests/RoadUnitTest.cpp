/*
 * RoadUnitTest.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: vagrant
 */

/*
 * IntersectionUnitTest.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: vagrant
 */
#include "../city/Road.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace PredictivePowertrain;

void road_ut() {

	// testing constructors
	Road test;
	int eleData = 1;
	int speedData = 1;
	std::string roadType = "angry";
	Road test2(roadType, &eleData, &speedData, 1);
	// test setStartNode(Intersection) ==
	test.setStartNode(NULL);
	assert(test.getStartNode() == NULL);
	//test setEndNode
	test.setEndNode(NULL);
	assert(test.getEndNode() == NULL);
	//test getRoadID
	assert(test.getRoadID() == 1);
	//test getSpeedData
	assert(*test.getSpeedData() == speedData);
	//test getElevData
	assert(*test.getElevData() == eleData);

}



