#ifndef PROBABILITY_H_
#define PROBABILITY_H_

namespace PredictivePowertrain {

class Probability{
private:
	float numerator;
	float denominator;

public:
	Probability();
	void addNumerator(float addition);
	void addDenominator(float addition);
	float getProbability();
};

}

#endif /* PROBABILITY_H_ */
