/*
 * GoalToLinkMapUnitTest.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#include "../route_prediction/GoalToLinkMap.h"
#include "../map/GenericMap.h"
#include "../route_prediction/GoalMapEntry.h"
#include "../driver_prediction/Link.h"
#include "../route_prediction/Goal.h"

void goalToLinkMap_UT() {
	// unit test for the GoalToLinkMap class
	int bins[] = {1};
	Goal goal1(1, bins);
	Goal goal2(2, bins);

	// Test 1: adding links to a single goal

	// Test 2: adding links to multiple goals

	// Test 2.5: single goal, multiple links

	// Test 3: getting probabilities with similar goals

	// Test 4: getting probabilities with dissimilar goals

	// Test 5: getting probabilities with some similar and some not so similar goals
}

