/*
 * speed_prediction.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef SPEED_PREDICTION_SPEED_PREDICTION_H_
#define SPEED_PREDICTION_SPEED_PREDICTION_H_

#include <Eigen/Dense>

using Eigen::MatrixXd;

namespace speed_prediction {

class speed_prediction {
private:
	double alpha; 			// learning rate
	int I;					// number of input units
	int *HN;				// number of neurons in each hidden layer
	int HL;					// number of hidden layers
	int O;					// number of output units
	int *totalLayers;		// total number of layers
	Eigen::MatrixXd * Wts;	// Matrix of weights
	double * spd_trc[];		// input speed trace
public:
	speed_prediction();
	speed_prediction(Eigen::MatrixXd * Wts);
	void init_params();
};

}

#endif /* SPEED_PREDICTION_SPEED_PREDICTION_H_ */
