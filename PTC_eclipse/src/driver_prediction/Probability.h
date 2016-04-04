#ifndef PROBABILITY_H_
#define PROBABILITY_H_

class Probability{
public:
	int numerator;
	int denominator;
	Probability();
	void addNumerator(int);
	void addDenominator(int);
	double getProbability();
};

#endif /* PROBABILITY_H_ */
