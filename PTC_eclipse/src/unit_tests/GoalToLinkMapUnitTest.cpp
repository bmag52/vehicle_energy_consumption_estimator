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
#include <iostream>
#include <assert.h>

using namespace PredictivePowertrain;

Link* makeLinks(int numberOfLinks) {
	Link* links = new Link[numberOfLinks];
	for(int i = 0; i < numberOfLinks; i++) {
		Link newLink(i, rand() > .5);
		links[i] = newLink;
	}
	return links;
}

void goalToLinkMap_UT() {
	// unit test for the GoalToLinkMap class
	Link* links = makeLinks(5);
	int bins1[] = {1};
	int bins2[]  = {1, 2};
	Goal goal1(1, bins1, 1);
	Goal goal2(2, bins2, 2);
	Goal goal3(3, bins1, 1);

	// Test 1: adding links to a single goal
	GoalToLinkMap map;
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link1 is not 1
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link1 is not 1
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link1 is not 1

	// Test 2: adding links to multiple goals
	map = GoalToLinkMap();
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link[0], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal2) == 1); // count for link[0], goal2 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link[1], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal2) == 1); // count for link[1], goal2 is not 1 after first add

	// Test 2.5: single goal, multiple links
	map = GoalToLinkMap();
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link[0], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link[1], goal1 is not 1 after first add
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 1); // probability for link[0], goal1 is not 1
	assert(map.probabilityOfGoalGivenLink(&(links[1]), &goal1, false) == 1); // probability for link[1], goal1 is not 1

	// Test 3: getting probabilities with dissimilar goals
	map = GoalToLinkMap();
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link[0], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal2) == 1); // count for link[0], goal2 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link[1], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal2) == 1); // count for link[0], goal2 is not 1 after first add
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 0.5); //probability of goal 1 when all goals are considered is not 0.5 when at link[0
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, true) == 1); //probability of goal 1 when similar goals are considered is not 1 when at link[0]

	// Test 4: getting probabilities with similar goals
	map = GoalToLinkMap();
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link[0], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal3) == 1); // count for link[0], goal3 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link[1], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal3) == 1); // count for link[1], goal3 is not 1 after first add
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 0.5); //probability of goal 1 when all goals are considered is not 0.5 when at link[0]
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, true) == 0.5); //probability of goal 1 when similar goals are considered is not 0.5 when at link[0

	// Test 5: getting probabilities with some similar and some not so similar goals
	map = GoalToLinkMap();
	assert(map.linkTraversed(&(links[0]), &goal1) == 1); // count for link[0], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal3) == 1); // count for link[0], goal3 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 1); // count for link[1], goal1 is not 1 after first add
	assert(map.linkTraversed(&(links[2]), &goal3) == 1); // count for link[2], goal3 is not 1 after first add
	assert(map.linkTraversed(&(links[0]), &goal1) == 2); // count for link[0], goal1 is not 2 after first add
	assert(map.linkTraversed(&(links[0]), &goal2) == 1); // count for link[1], goal2 is not 1 after first add
	assert(map.linkTraversed(&(links[1]), &goal1) == 2); // count for link[1], goal1 is not 2 after first add
	assert(map.linkTraversed(&(links[2]), &goal2) == 1); // count for link[2], goal2 is not 1 after first add
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 0.5); //probability of goal 1 when all goals are considered is not 0.5 when at link[0]
	//CONVERT INTEGER FRACTIONS TO DOUBLE??
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == (2/3)); //probability of goal 1 when all goals are considered is not 2/3 when at link[0]

	// Test 6: return value for unseen transition is zero, with nothing in the map
	map = GoalToLinkMap();
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 0); // returned value is non-zero

	// Test 7: return value for unseen transition is zero with things in the map
	map = GoalToLinkMap();
	map.linkTraversed(&(links[0]), &goal1);
	assert(map.probabilityOfGoalGivenLink(&(links[1]), &goal1, false) == 0); // returned value is non-zero
	assert(map.probabilityOfGoalGivenLink(&(links[0]), &goal2, false) == 0); // returned value is non-zero
	assert(map.probabilityOfGoalGivenLink(&(links[1]), &goal2, false) == 0); // returned value is non-zero
}


