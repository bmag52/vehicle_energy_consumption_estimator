/*
 * ElevationTypesUnitTest.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */
#include "../city/ElevationTypes.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"
#include <string>

using namespace PredictivePowertrain;

void elevationtypes_ut(){
	/*        no_type (5, 100, .1, .7, 'no_type') %basic qualities
        flat (3, 20, .6, .7, 'flat') %long sections, low grade
        moderate (5, 500, .3, .6, 'moderate') %medium sections, moderate grade
        hilly (10, 100, .3, .5, 'hilly') %short sections, high grade
        mountainous (10, 50, .5, .7, 'mountainous') %long sections, high grade*/
	//testing initialization of all types
	ElevationTypes noTypeTest_1(5, 100, .1, .7, "no_type");
	ElevationTypes noTypeTest_2("no_type");
	assert(noTypeTest_1.grade == noTypeTest_2.grade);
	assert(noTypeTest_1.deviation == noTypeTest_2.deviation);
	assert(noTypeTest_1.max_section_length == noTypeTest_2.max_section_length);
	assert(noTypeTest_1.min_section_length == noTypeTest_2.min_section_length);

	ElevationTypes flatTest_1(3, 20, .6, .7, "flat");
	ElevationTypes flatTest_2("flat");
	assert(flatTest_1.grade != noTypeTest_2.grade);
	assert(flatTest_1.grade == flatTest_2.grade);

	ElevationTypes modTest_1(5, 500, .3, .6, "moderate");
	ElevationTypes modTest_2("moderate");
	assert(modTest_1.grade == modTest_2.grade);

	ElevationTypes hillyTest_1(10, 100, .3, .5, "hilly");
	ElevationTypes hillyTest_2("hilly");
	assert(hillyTest_1.grade == hillyTest_2.grade);

	ElevationTypes mountTest_1(10, 50, .5, .7, "mountainous");
	ElevationTypes mountTest_2("mountainous");
	assert(mountTest_1.grade == mountTest_2.grade);
}



