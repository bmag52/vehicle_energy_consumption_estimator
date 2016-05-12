#include "../driver_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include <stdlib.h>
#include <assert.h>
#include "../route_prediction/LinkToStateMap.h"
#include "UnitTests.h"
#include <iostream>

using namespace PredictivePowertrain;
using namespace std;

void linkToStateMap_ut() {
	int linksSize = 5;
	Link* links[linksSize];
	srand(time(NULL));
	for(int i = 0; i < linksSize; i++) {
		int random = rand() % 5;
		Link newLink(i, random);
		links[i] = &newLink;
	}

	int bins2[] = {2};
	int bins4[] = {4};

	Goal goal1(1, bins2);
	Goal goal2(2, bins4);
	Goal goal3(2, bins2);
	assert(goal1.isSimilar(&goal3)); // goal 1 and goal3 should be similar

	// Test 1: increment transition
	LinkToStateMap map;
	assert(map.incrementTransition(links[0], &goal1, links[1]) == 1);

	// Test 2: increment transition twice
	LinkToStateMap map2;
	int count = 0;
	for(int i = 0; i < 2; i++) {
		count += map2.incrementTransition(links[1], &goal2, links[3]);
	}
	assert(count == 1);

	// Test 3: increment two different transitions
	LinkToStateMap map3;
	int count1 = map3.incrementTransition(links[3], &goal1, links[0]);
	int count2 = map3.incrementTransition(links[4], &goal2, links[1]);
	assert(count1 == 1 && count1 == count2);

	// Test 4: get probability with nothing in map
	LinkToStateMap map4;
	assert(map4.getProbability(links[0], links[1], &goal1, true) == 0);
	map4.incrementTransition(links[2], &goal1, links[1]);

	assert(map4.getProbability(links[0], links[1], &goal1, true) == 0);
	assert(map4.getProbability(links[2], links[0], &goal1, true) == 0);
	assert(map4.getProbability(links[0], links[0], &goal1, true) == 0);

	// Test 5: get probability when only one goal in map
	LinkToStateMap map5;
	map5.incrementTransition(links[0], &goal1, links[1]);
	map5.incrementTransition(links[0], &goal1, links[2]);
	assert(map.getProbability(links[1], links[0], &goal1, false) == 0);

	// Test 6: get probability when other goals in map
	LinkToStateMap map6;
	map6.incrementTransition(links[0], &goal1, links[1]);
	map6.incrementTransition(links[0], &goal1, links[2]);
	map6.incrementTransition(links[0], &goal2, links[1]);
	assert(map6.getProbability(links[1], links[0], &goal1, false) == 0.5);

	// Test 7: test issimilar for getprobability
	LinkToStateMap map7;
	map7.incrementTransition(links[0], &goal1, links[1]);
	map7.incrementTransition(links[0], &goal1, links[2]);
	map7.incrementTransition(links[0], &goal3, links[1]);

	assert(map7.getProbability(links[1], links[0], &goal1, false) == 0.5);
	double pl = map7.getProbability(links[1], links[0], &goal1, true);
	assert(pl == (2.0/3.0));  // says it is == 1

	// Test 8: test issimilar for getprobability with non similar goals
	LinkToStateMap map8;
	map8.incrementTransition(links[4], &goal1, links[1]);
	map8.incrementTransition(links[4], &goal1, links[2]);
	map8.incrementTransition(links[4], &goal3, links[1]);
	map8.incrementTransition(links[4], &goal2, links[1]);

	assert(map8.getProbability(links[1], links[4], &goal1, false) == 0.5);
	double pl2 = map8.getProbability(links[1], links[4], &goal1, true);
	assert(pl2 == (2.0/3.0));

	// Test 9: unseen transition returns 0
	LinkToStateMap map9;
	assert(map9.getProbability(links[1], links[4], &goal1, false) == 0);

	// No copy function in LTSM
/*	// Test 10: copy
	LinkToStateMap map10;
	map10.incrementTransition(links[4], goal1, links[1]);
	map10.incrementTransition(links[4], goal1, links[2]);
	map10.incrementTransition(links[4], goal3, links[1]);
	map10.incrementTransition(links[4], goal2, links[1]);

	LinkToStateMap map10Copy = map10.copy();
	assert(map10.getProbability(links[1], links[4], goal1, false) ==
			map10Copy.getProbability(links[1], links[4], goal1, false));
	map10.incrementTransition(links[4], goal1, links[1]);
	assert(map10.getProbability(links[1], links[4], goal1, false) !=
				map10Copy.getProbability(links[1], links[4], goal1, false));*/
}
