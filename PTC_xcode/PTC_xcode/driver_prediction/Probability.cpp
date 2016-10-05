//
#include "Probability.h"

namespace PredictivePowertrain {

Probability::Probability()
{
	this->numerator = 0;
	this->denominator = 0;
}

void Probability::addNumerator(float addition)
{
	this->numerator = this->numerator + addition;
}

void Probability::addDenominator(float addition)
{
	this->denominator = this->denominator + addition;
}

float Probability::getProbability()
{
	if(this->denominator == 0)
    {
		return 0.0;
    }
    
	return this->numerator/this->denominator;
}

}

