/*
 * CitySectionUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "../city/CitySection.h"
#include <iostream>
#include <assert.h>
#include <string>
#include "UnitTests.h"

using namespace PredictivePowertrain;

void citySection_ut(){

	CitySection test1("city", 1, 1);
	CitySection test2("default", 20, 20);
	CitySection test3("country", 1, 1);

	assert(test1.x_dim == test3.x_dim);
	assert(test1.x_dim == test1.y_dim);

	assert(test2.roadType != test3.roadType);
	assert(test2.x_dim == 20 && test2.y_dim == 20);

	assert(test3.y_dim == test1.y_dim);
	assert(test3.y_dim != test2.y_dim);




}
