/*
 * IntersectionTypesUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../city/IntersectionTypes.h"
#include <iostream>
#include <assert.h>
#include <string>

using namespace PredictivePowertrain;

void intersectiontypes_ut(){
	std::string def = "default";
	std::string stopLight = "stop light";
	std::string stopSign = "stop sign";
	std::string destination = "destination";
	std::string exit = "exit";

	IntersectionTypes defaultType(def, 0, 10);
	IntersectionTypes stopLightType(stopLight, 0, 30);
	IntersectionTypes stopSignType(stopSign, 5, 10);
	IntersectionTypes destinationType(destination, 0, 0);
	IntersectionTypes exitType(exit, 0 , 10);

	IntersectionTypes defaultType_2(def);
	IntersectionTypes stopLightType_2(stopLight);
	IntersectionTypes stopSignType_2(stopSign);
	IntersectionTypes destinationType_2(destination);
	IntersectionTypes exitType_2(exit);

	assert(!defaultType.toString.compare(defaultType_2.toString));
	assert(!stopLightType.toString.compare(stopLightType_2.toString));
	assert(!stopSignType.toString.compare(stopSignType_2.toString));
	assert(!destinationType.toString.compare(destinationType_2.toString));
	assert(!exitType.toString.compare(exitType_2.toString));

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

	assert(exitType.stop_time("exit", &defaultType, &defaultType_2) == 0);
	assert(exitType.stop_time("exit", &destinationType, &exitType) != 0);

}


