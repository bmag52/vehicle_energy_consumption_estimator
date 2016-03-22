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
	int size;
public:
	Goal();
	Goal(int destination, int bin[], int size);
	Goal(Goal& other);
	bool issimilar(Goal &other);
	bool isequal(Goal &other);
	size_t get_hash() const;
	virtual ~Goal();
};

} /* namespace RoutePrediction */

#endif /* DRIVER_PREDICTION_GOAL_H_ */
