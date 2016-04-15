/*
 * GoalToLinkMapUnitTest.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#include "../route_prediction/GoalToLinkMap.h"
#include "UnitTests.h"
#include <iostream>
#include <assert.h>

using namespace PredictivePowertrain;

Link* makeLinks(int numberOfLinks)
{
	Link* links = new Link[numberOfLinks];
	for(int i = 0; i < numberOfLinks; i++)
	{
		Link newLink(i, rand() > .5);
		links[i] = newLink;
	}
}

void goalToLinkMap_ut() {
	Link* links = makeLinks(5);
	int bins1[] = {1};
	int bins2[]  = {1, 2};
	Goal goal1(1, bins1, 1);
	Goal goal2(2, bins2, 2);
	Goal goal3(3, bins1, 1);

	// Test 1: adding links to a single goal
	GoalToLinkMap map1;
	assert(map1.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map1.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map1.linkTraversed(&(links[0]), &goal1) == 1);

	// Test 2: adding links to multiple goals
	GoalToLinkMap map2;
	assert(map2.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map2.linkTraversed(&(links[0]), &goal2) == 1);
	assert(map2.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map2.linkTraversed(&(links[1]), &goal2) == 1);

	// Test 2.5: single goal, multiple links
	GoalToLinkMap map3;
	assert(map3.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map3.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map3.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 1);
	assert(map3.probabilityOfGoalGivenLink(&(links[1]), &goal1, false) == 1);

	// Test 3: getting probabilities with dissimilar goals
	GoalToLinkMap map4;
	assert(map4.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map4.linkTraversed(&(links[0]), &goal2) == 1);
	assert(map4.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map4.linkTraversed(&(links[0]), &goal2) == 1);
	assert(map4.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == (1/2));
	assert(map4.probabilityOfGoalGivenLink(&(links[0]), &goal1, true) == 1);

	// Test 4: getting probabilities with similar goals
	GoalToLinkMap map5;
	assert(map5.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map5.linkTraversed(&(links[0]), &goal3) == 1);
	assert(map5.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map5.linkTraversed(&(links[1]), &goal3) == 1);
	assert(map5.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == (1/2));
	assert(map5.probabilityOfGoalGivenLink(&(links[0]), &goal1, true) == (1/2));

	// Test 5: getting probabilities with some similar and some not so similar goals
	GoalToLinkMap map6;
	assert(map6.linkTraversed(&(links[0]), &goal1) == 1);
	assert(map6.linkTraversed(&(links[0]), &goal3) == 1);
	assert(map6.linkTraversed(&(links[1]), &goal1) == 1);
	assert(map6.linkTraversed(&(links[2]), &goal3) == 1);
	assert(map6.linkTraversed(&(links[0]), &goal1) == 2);
	assert(map6.linkTraversed(&(links[0]), &goal2) == 1);
	assert(map6.linkTraversed(&(links[1]), &goal1) == 2);
	assert(map6.linkTraversed(&(links[2]), &goal2) == 1);
	assert(map6.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == (1/2));
	assert(map6.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == (2/3));

	// Test 6: return value for unseen transition is zero, with nothing in the map
	GoalToLinkMap map7;
	assert(map7.probabilityOfGoalGivenLink(&(links[0]), &goal1, false) == 0);

	// Test 7: return value for unseen transition is zero with things in the map
	GoalToLinkMap map8;
	map8.linkTraversed(&(links[0]), &goal1);
	assert(map8.probabilityOfGoalGivenLink(&(links[1]), &goal1, false) == 0);
	assert(map8.probabilityOfGoalGivenLink(&(links[0]), &goal2, false) == 0);
	assert(map8.probabilityOfGoalGivenLink(&(links[1]), &goal2, false) == 0);
}


