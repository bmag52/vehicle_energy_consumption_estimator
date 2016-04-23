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

void roadTypes_ut() {
	//Testing constructor w/ adjusted variable values
	RoadTypes city("red", 25, 55, 500, 1000, "city");

	//Testing adjusted roadTypes objects
	assert(city.color == "red");
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
	assert(defaultType.color == "yellow" && cityType.color == "red");
	assert(countryType.color == "blue" && interstateType.color == "black");

	//Testing set roadTypes min speed
	assert(defaultType.min_speed == 25 && cityType.min_speed == 25);
	assert(countryType.min_speed == 40 && interstateType.min_speed == 95);

	//Testing set roadTypes max speed
	assert(defaultType.max_speed == 130 && cityType.max_speed == 55);
	assert(countryType.max_speed == 90 && interstateType.max_speed == 130);

	//Testing set roadTypes min distance
	assert(defaultType.min_distance == 0 && cityType.min_distance == 500);
	assert(countryType.min_distance == 1000 && interstateType.min_distance == 1000);

	//Testing set roadTypes max distance
	assert(defaultType.max_distance == numeric_limits<int>::max() && cityType.max_distance == 1000);
	assert(countryType.max_distance == 2000 && interstateType.max_distance == numeric_limits<int>::max());

	//Testing set roadTypes toString
	//No to_String test for default and interstate b/c of numeric_limits<int>::max()
	assert(cityType.toString == "Road Type: city Color: red Min Speed: 25 Max Speed: 55 Min Distance: 500 Max Distance: 1000");
	assert(countryType.toString == "Road Type: country Color: blue Min Speed: 40 Max Speed: 90 Min Distance: 1000 Max Distance: 2000");

}


