//
#include "Probability.h"
Probability::Probability() {
	numerator = 0;
	denominator = 0;
}

void Probability::addNumerator(int addition){
	numerator = numerator + addition;
}

void Probability::addDenominator(int addition){
	denominator = denominator + addition;
}

double Probability::getProbability(){
	if(denominator == 0)
		return 0.0;
	return (double)numerator/denominator;
}
// add num
// add denom
// return prob(if denom = 0, prob = 0
