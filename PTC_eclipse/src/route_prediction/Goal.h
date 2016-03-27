/*
 * Goal.h
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#ifndef DRIVER_PREDICTION_GOAL_H_
#define DRIVER_PREDICTION_GOAL_H_

#include <functional>

namespace RoutePrediction {

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
	size_t getHash() const;
	int getNumSeen();
	void setNumSeen(int numSeen);
	void incrementNumSeen();
	virtual ~Goal();
};

} /* namespace RoutePrediction */

#endif /* DRIVER_PREDICTION_GOAL_H_ */
