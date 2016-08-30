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

namespace PredictivePowertrain {

class Goal {
private:
	int destination;
	int *bins;
	int numSeen;
	int size;
public:
	Goal();
	Goal(int destination);
	Goal(int destination, int* bin);
	Goal(int destination, int bin[], int size);
	Goal(Goal * other);
	bool isSimilar(Goal * other);
	bool isEqual(Goal * other);
	int getHash() const;
	int getNumSeen();
	void setNumSeen(int numSeen);
	void incrementNumSeen();
	int* getBins();
	virtual ~Goal();
};

} /* namespace PredictivePowertrain */

#endif /* DRIVER_PREDICTION_GOAL_H_ */
