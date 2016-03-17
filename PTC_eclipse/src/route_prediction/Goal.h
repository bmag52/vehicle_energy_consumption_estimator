/*
 * Goal.h
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#ifndef DRIVER_PREDICTION_GOAL_H_
#define DRIVER_PREDICTION_GOAL_H_

#include <functional>

namespace InnovationModel {

class Goal {
private:
	int destination;
	int *bins;
	int size;
public:
	Goal(int destination, int bin[], int size);
	Goal(Goal& other);
	bool issimilar(Goal &other);
	bool isequal(Goal &other);
	size_t get_hash() const;
	virtual ~Goal();
};

} /* namespace InnovationModel */

#endif /* DRIVER_PREDICTION_GOAL_H_ */
