/*
 * RuteUnitTest.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: vagrant
 */

#include "../driver_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include "../route_prediction/Route.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;
using namespace DriverPrediction;
using namespace RoutePrediction;

void route_UT() {
	Link * zeroLinks = {};
	Link * firstLinks = {new Link(10, 20)};

	int zeroBins[0] = {};
	int firstBins[3] = {4, 7, 3};
	Goal goal1 = new Goal(2, zeroBins, 4);
	Goal goal2 = new Goal(2, firstBins, 4);


	// Test add link
	Route * addLinkTest1 = new Route(zeroLinks, &goal1);
	Link * test = {new Link(15, 30)};
	(*addLinkTest1).addlink(test);
	assert(((*addLinkTest1).getEntry(1))->getHash() == (*test).getHash());
	assert((*addLinkTest1).getLinkSize() == 0);

	// Test isequal

	// Test copy

	// Test nextlink



	/*int firstBins[0] = {};
	int secondBins[1] = {10};
	Goal firstGoal(0, firstBins, 0);
	Goal secondGoal(10, secondBins, 1);
	//Testing route(x, x) constructor, isequal()
	Route firstRoute(firstLinks, firstGoal);
	Route secondRoute(secondLinks, secondGoal);
	Route thirdRoute(secondLinks, secondGoal);
	assert(secondRoute.isequal(thirdRoute));
	//assert(secondRoute.isequal(firstRoute));
	//Testing addlink(Link) function
	Route fourthRoute(firstLinks, secondGoal);
	fourthRoute.addlink(Link(10, 20));
	assert(fourthRoute.isequal(secondRoute));
	//Testing copy() function
	//Route fifthRoute = secondRoute.copy();
	//assert(fifthRoute.isequal(secondRoute));
	//Testing nextLink() function
	assert((secondRoute.nextlink()).isEqual(secondLinks[0]));
	assert((secondRoute.nextlink()).isEqual(secondLinks[0].final_link()));*/

}/*
 * RuteUnitTest.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: vagrant
 */

#include "../driver_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include "../route_prediction/Route.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;
using namespace DriverPrediction;
using namespace RoutePrediction;

void route_UT() {
	Link * zeroLinks = {};
	Link * firstLinks = {new Link(10, 20)};

	int zeroBins[0] = {};
	int firstBins[3] = {4, 7, 3};
	Goal goal1 = new Goal(2, zeroBins, 4);
	Goal goal2 = new Goal(2, firstBins, 4);


	// Test add link
	Route * addLinkTest1 = new Route(zeroLinks, &goal1);
	Link * test = {new Link(15, 30)};
	(*addLinkTest1).addlink(test);
	assert(((*addLinkTest1).getEntry(1))->getHash() == (*test).getHash());
	assert((*addLinkTest1).getLinkSize() == 0);

	// Test isequal

	// Test copy

	// Test nextlink



	/*int firstBins[0] = {};
	int secondBins[1] = {10};
	Goal firstGoal(0, firstBins, 0);
	Goal secondGoal(10, secondBins, 1);
	//Testing route(x, x) constructor, isequal()
	Route firstRoute(firstLinks, firstGoal);
	Route secondRoute(secondLinks, secondGoal);
	Route thirdRoute(secondLinks, secondGoal);
	assert(secondRoute.isequal(thirdRoute));
	//assert(secondRoute.isequal(firstRoute));
	//Testing addlink(Link) function
	Route fourthRoute(firstLinks, secondGoal);
	fourthRoute.addlink(Link(10, 20));
	assert(fourthRoute.isequal(secondRoute));
	//Testing copy() function
	//Route fifthRoute = secondRoute.copy();
	//assert(fifthRoute.isequal(secondRoute));
	//Testing nextLink() function
	assert((secondRoute.nextlink()).isEqual(secondLinks[0]));
	assert((secondRoute.nextlink()).isEqual(secondLinks[0].final_link()));*/

}
