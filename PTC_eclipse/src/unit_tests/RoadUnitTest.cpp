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

using namespace std;


void road_ut() {

	// testing constructors
	Road test;
	Road test2(default, 1, 1, 1);
	// test setStartNode(Intersection) ==
	test.setStartNode(null);
	assert(test.getStartNode() == null);
	//test setEndNode
	test.setEndNode(null);
	assert(test.setEndNode() == null);
	//test getRoadID
	assert(test.getRoadID() == 1);
	//test getSpeedData
	assert(test.getSpeedData() == 1);
	//test getElevData
	assert(test.getElevData() == 1);

}



