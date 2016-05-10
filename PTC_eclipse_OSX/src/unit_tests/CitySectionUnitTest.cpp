/*
 * CitySectionUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../city/CitySection.h"
#include <iostream>
#include <assert.h>
#include <string>

using namespace PredictivePowertrain;

void citySection_ut() {

	CitySection* test1 = new CitySection("city", 1, 1);
	CitySection* test2 = new CitySection("default", 20, 20);
	CitySection* test3 = new CitySection("country", 1, 1);

	assert(test1->x_dim == test3->x_dim);
	assert(test1->x_dim == test1->y_dim);

	assert(test2->roadType != test3->roadType);
	assert(test2->x_dim == 20 && test2->y_dim == 20);

	assert(test3->y_dim == test1->y_dim);
	assert(test3->y_dim != test2->y_dim);

}
