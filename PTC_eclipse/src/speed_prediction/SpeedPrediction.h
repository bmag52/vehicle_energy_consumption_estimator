/*
 * speed_prediction.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef SPEED_PREDICTION_SPEEDPREDICTION_H_
#define SPEED_PREDICTION_SPEEDPREDICTION_H_

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <assert.h>

namespace PredictivePowertrain {

class SpeedPrediction {
private:
	int I;						// number of input units
	int HL;						// number of hidden layers
	int O;						// number of output units
	int lastLayer;				// number of the last layer
	int maxSpeed;				// max vehicle speed
	int *HN;					// number of neurons in each hidden layer
	int *totalLayers;			// total number of layers
	double lb_offset;			// lower bound offset
	double alpha; 				// learning rate
	Eigen::MatrixXd * Wts;		// matrix of weights
	Eigen::MatrixXd * yHid;		// hidden layer outputs
	Eigen::MatrixXd * yInHid;	// hidden layer inputs

	void initParams();
	void printAll();
public:
	SpeedPrediction();
	SpeedPrediction(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
	void predict(Eigen::MatrixXd * spd_in, Eigen::MatrixXd * spd_out);
	void train(Eigen::MatrixXd * spd_pred, Eigen::MatrixXd * spd_act, Eigen::MatrixXd * spd_in);
	void getVals(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
	int getI();
	int getO();
	void formatInData(Eigen::MatrixXd * input);
	void formatOutData(Eigen::MatrixXd * output);
};

}

#endif /* SPEED_PREDICTION_SPEEDPREDICTION_H_ */
