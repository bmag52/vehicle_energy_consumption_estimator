#ifndef PROBABILITY_H_
#define PROBABILITY_H_

namespace PredictivePowertrain {

class Probability{
private:
	int numerator;
	int denominator;

public:
	Probability();
	void addNumerator(int);
	void addDenominator(int);
	double getProbability();
};

}

#endif /* PROBABILITY_H_ */
