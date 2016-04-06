/*
 * Probablity.h
 *
 *  Created on: Mar 31, 2016
 *      Author: vagrant
 */

#ifndef DRIVER_PREDICTION_PROBABLITY_H_
#define DRIVER_PREDICTION_PROBABLITY_H_

namespace DriverPrediction {

class Probablity {
public:
	int numerator;
	int denominator;
	Probablity();
	Probablity(int, int);
	void addNumerator(int);
	void addDenominator(int);
	double getProbablity();
};

} //namespace


#endif /* DRIVER_PREDICTION_PROBABLITY_H_ */
