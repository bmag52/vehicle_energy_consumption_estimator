/*
 * GoalMapEntryUnitTest.cpp
 *
 *  Created on: Apr 11, 2016
 *      Author: vagrant
 */
#include "../route_prediction/GoalMapEntry.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

//using namespace std;
using namespace PredictivePowertrain;

void goalmapentry_ut(){
	GoalMapEntry<int, int> testGoal;
	// test increment method
	testGoal.incrementCount();
	assert(testGoal.getM() == 1);

	//test goal constructor
	Goal test(1);
	GoalMapEntry<int, int> testGoal2(&test);
	assert(testGoal.getGoal() == &test);
}



