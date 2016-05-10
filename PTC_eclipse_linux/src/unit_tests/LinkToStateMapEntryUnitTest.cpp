/*
 * LinkToStateMapEntryUnitTest.cpp
 *
 *  Created on: Apr 13, 2016
 *      Author: vagrant
 */

#include "../route_prediction/LinkToStateMapEntry.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;
using namespace PredictivePowertrain;


void linkToStateMapEntry_ut() {

	//Testing constructors, getTotalM(), totalM()
	LinkToStateMapEntry first;
	LinkToStateMapEntry second(first);

	assert(first.getTotalM()==0 && second.getTotalM() == 0);
	assert(first.totalM(&second) == 0);

	//Testing addEntry()
	Link link1;
	Link link2(10,20);
	first.addEntry(&link1);
	assert(first.getM(&link1) == 1);
	assert(first.getTotalM() == 1);
	assert(second.getM(&link1) == 0);

	first.addEntry(&link2);
	assert(first.getTotalM() == 2);
	assert(second.getTotalM() == 0);
	assert(second.totalM(&first) == 2);

	//updating an entry, increasing m
	first.addEntry(&link1);
	assert(first.getM(&link1) == 2);
	assert(first.getTotalM() == 3);
	assert(second.totalM(&first) == 3);

}

