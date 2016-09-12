/*
 * GoalUnitTest.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: vagrant
 */
#include "../route_prediction/Goal.h"
#include <iostream>
#include <assert.h>
#include <vector>
#include "UnitTests.h"

using namespace PredictivePowertrain;

void goal_ut(){
	// testing constructors (int destination, int [] bin, int size)
	Goal goalOne;
	Goal goalTwo(1);
    std::vector<float> arr = {0};
	Goal goalThree(1, &arr);
	Goal goalFour(2, &arr);
	Goal goalFive(goalFour);
	Goal goalSix(3, &arr);

	// test isSimilar (doesn't compare destination)
	assert(goalFour.isSimilar(&goalSix));	// true
	assert(!goalFour.isSimilar(&goalOne));	// !false
	// test isEquals (compares all attributes)
	assert(goalFour.isEqual(&goalFive));		// true
	assert(!goalFour.isEqual(&goalSix));		// !false


	// get,set & increment num seen
	goalOne.setNumSeen(1);
	int oldNum = goalOne.getNumSeen();
	goalOne.incrementNumSeen();
	assert(oldNum + 1 == goalOne.getNumSeen());

	// getBins()
    std::vector<float>* bins = goalThree.getBins();
	assert(arr.at(1) == bins->at(1));
}




