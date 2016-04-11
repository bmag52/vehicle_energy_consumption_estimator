//
#include "Probability.h"

namespace PredictivePowertrain {

Probability::Probability() {
	this->numerator = 0;
	this->denominator = 0;
}

void Probability::addNumerator(int addition){
	this->numerator = this->numerator + addition;
}

void Probability::addDenominator(int addition){
	this->denominator = this->denominator + addition;
}

double Probability::getProbability(){
	if(this->denominator == 0)
		return 0.0;
	return (double)this->numerator/this->denominator;
}

}

