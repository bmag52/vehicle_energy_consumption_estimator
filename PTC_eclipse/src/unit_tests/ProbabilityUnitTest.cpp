/*
 * ProbabilityUnitTest.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: vagrant
 */
#include "../driver_prediction/Probability.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;


void probability_ut() {
	Probability firstProb;
	// testing constructor(numerator = 0 & denom = 0)
	// testing getProbability method if denom = 0
	assert(firstProb.getProbability == 0.0);

	// testing addDenominator & getProbability, (double)0/1 == 0.0
	firstProb.addDenominator(1);
	assert(firstProb.getProbability == 0.0);

	// testing addDenominator & getProbability, (double)1/1 == 1.0
	firstProb.addNumerator(1);
	assert(firstProb.getProbability == 1.0);
	assert(firstProb.getProbability != 0.0);

}




