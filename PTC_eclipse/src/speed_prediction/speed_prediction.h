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
	double alpha; 				// learning rate
	int I;						// number of input units
	int *HN;					// number of neurons in each hidden layer
	int HL;						// number of hidden layers
	int O;						// number of output units
	int *totalLayers;			// total number of layers
	Eigen::MatrixXd * Wts;		// matrix of weights
	Eigen::MatrixXd yHid;		// hidden layer outputs
	Eigen::MatrixXd yInHid;	// hidden layer inputs
public:
	speed_prediction();
	speed_prediction(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
	void init_params();
	void predict(double * spd_in, double * spd_out);
	void train(double * spd_pred, double * spd_act);
	void get_vals(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
};

}

#endif /* SPEED_PREDICTION_SPEED_PREDICTION_H_ */
