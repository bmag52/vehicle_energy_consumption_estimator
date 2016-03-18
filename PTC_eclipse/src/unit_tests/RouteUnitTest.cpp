/*
 * RuteUnitTest.cpp
 *
 *  Created on: Mar 17, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../route_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include "../driver_prediction/Route.h"
#include <iostream>
#include <assert.h>

using namespace InnovationModel;

void route_UT() {
	Link firstLinks[0] = {};
	Link secondLinks[1] = {Link(10, 20)};

	int firstBins[0] = {};
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
	assert((secondRoute.nextlink()).isEqual(secondLinks[0].final_link()));

}



