/*
 * Goal.h
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#ifndef DRIVER_PREDICTION_GOAL_H_
#define DRIVER_PREDICTION_GOAL_H_

#include <functional>
#include <cmath>
#include <vector>

namespace PredictivePowertrain {

class Goal {
private:
	long destination;
    std::vector<float>* bins;
	int numSeen;
public:
	Goal();
	Goal(long int destination);
    Goal(long int destination, std::vector<float>* bins);
	Goal(Goal * other);
	bool isSimilar(Goal * other);
	bool isEqual(Goal * other);
	long int getHash() const;
	int getNumSeen();
	void setNumSeen(int numSeen);
	void incrementNumSeen();
    std::vector<float>* getBins();
	virtual ~Goal();
};

} /* namespace PredictivePowertrain */

#endif /* DRIVER_PREDICTION_GOAL_H_ */
