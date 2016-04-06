/*
 * Probablity.cpp
 *
 *  Created on: Mar 31, 2016
 *      Author: vagrant
 */

#include "Probablity.h"
#include <math.h>

using namespace std;

namespace DriverPrediction {

Probablity::Probablity() {
	numerator = 0;
	denominator = 0;
}

Probablity::Probablity(int num, int denom) {
	numerator = num;
	denominator = denom;
}

void Probablity::addNumerator(int addition) {
	numerator += addition;
}

void Probablity::addDenominator(int addition) {
	denominator += addition;
}

double Probablity::getProbablity() {
	if (denominator == 0) {
		return 0.0;
	} else {
		return (double) numerator/denominator;
	}
}

int main() {
	Probablity prob(5,5);
	prob.addDenominator(5);
	prob.addNumerator(5);
}

}


