/*
 * RoadTypesUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "../city/RoadTypes.h"
#include "UnitTests.h"
#include <assert.h>
#include <string>
#include <limits>

using namespace PredictivePowertrain;
using namespace std;

void road_ut() {
	//Testing constructor w/ adjusted variable values
	RoadTypes city("red", 25, 55, 500, 1000, "city"); //city street, 22 - 55 kph

	//Testing adjusted roadTypes objects
	assert(city.color == "yellow");
	assert(city.min_speed == 25);
	assert(city.max_speed == 55);
	assert(city.min_distance == 500);
	assert(city.max_distance == 1000);
	assert(city.toString == "city");

	//Testing constructor w/ set variable values
	RoadTypes defaultType("default");
	RoadTypes cityType("city");
	RoadTypes countryType("country");
	RoadTypes interstateType("interstate");

	//Testing set roadTypes color
	assert(defaultType.color == "yellow");
	assert(cityType.color == "red");
	assert(countryType.color == "blue");
	assert(interstateType.color == "black");

	//Testing set roadTypes min speed
	assert(defaultType.min_speed == 25);
	assert(cityType.min_speed == 25);
	assert(countryType.min_speed == 40);
	assert(interstateType.min_speed == 95);

	//Testing set roadTypes max speed
	assert(defaultType.max_speed == 130);
	assert(cityType.max_speed == 55);
	assert(countryType.max_speed == 90);
	assert(interstateType.max_speed == 130);

	//Testing set roadTypes min distance
	assert(defaultType.min_distance == 0);
	assert(cityType.min_distance == 500);
	assert(countryType.min_distance == 1000);
	assert(interstateType.min_distance == 1000);

	//Testing set roadTypes max distance
	assert(defaultType.max_distance == numeric_limits<int>::max());
	assert(cityType.max_distance == 1000);
	assert(countryType.max_distance == 2000);
	assert(interstateType.max_distance == numeric_limits<int>::max());

	//Testing set roadTypes toString
	assert(defaultType.toString == "default");
	assert(cityType.toString == "city");
	assert(countryType.toString == "country");
	assert(interstateType.toString == "interstate");


}


