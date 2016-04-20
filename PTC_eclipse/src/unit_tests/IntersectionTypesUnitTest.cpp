/*
 * IntersectionTypesUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "../city/IntersectionTypes.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"
#include <string>

using namespace PredictivePowertrain;

void intersectiontypes_ut(){
	IntersectionTypes defaultType("default", 0, 10);
	IntersectionTypes stopLightType("stop light", 0, 30);
	IntersectionTypes stopSignType("stop sign", 5, 10);
	IntersectionTypes destinationType("destination", 0, 0);
	IntersectionTypes exitType("exit", 0 , 10);

	IntersectionTypes defaultType_2("default");
	IntersectionTypes stopLightType_2("stop light");
	IntersectionTypes stopSignType_2("stop sign");
	IntersectionTypes destinationType_2("destination");
	IntersectionTypes exitType_2("exit");

	assert(defaultType.toString.equals(defaultType_2.toString));
	assert(stopLightType.toString.equals(stopLightType_2.toString));
	assert(stopSignType.toString.equals(stopSignType_2.toString));
	assert(destinationType.toString.equals(destinationType_2.toString));
	assert(exitType.toString.equals(exitType_2.toString));

	assert(defaultType.min_stop_time == defaultType_2.min_stop_time);
	assert(defaultType.min_stop_time == defaultType_2.min_stop_time);
	assert(stopSignType.min_stop_time == stopSignType_2.min_stop_time);
	assert(destinationType.min_stop_time == destinationType_2.min_stop_time);
	assert(exitType.min_stop_time == exitType_2.min_stop_time);

	assert(defaultType.max_stop_time == defaultType_2.max_stop_time);
	assert(defaultType.max_stop_time == defaultType_2.max_stop_time);
	assert(stopSignType.max_stop_time == stopSignType_2.max_stop_time);
	assert(destinationType.max_stop_time == destinationType_2.max_stop_time);
	assert(exitType.max_stop_time == exitType_2.max_stop_time);

	assert(exitType.stop_time("exit", defaultType, defaultType_2) == 0);
	assert(exitType.stop_time("exit", destinationType, exitType) != 0);

}


