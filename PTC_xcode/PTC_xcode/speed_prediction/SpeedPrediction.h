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
#include <list>
#include <ctime>
#include <vector>
#include "../route_prediction/Route.h"

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
	float lb_offset;			// lower bound offset
	float alpha; 				// learning rate
    float ds;                  // time delta between predicted values
    std::vector<Eigen::MatrixXd*>* Wts;         // matrix of weights
    std::vector<Eigen::MatrixXd*>* yHid;		// hidden layer outputs
    std::vector<Eigen::MatrixXd*>* yInHid;      // hidden layer inputs

	void initParams();
	void printAll();
    
public:
	SpeedPrediction();
	SpeedPrediction(std::vector<Eigen::MatrixXd*>* Wts, std::vector<Eigen::MatrixXd*>* yHid, std::vector<Eigen::MatrixXd*>* yInHid);
	void predict(Eigen::MatrixXd * spd_in, Eigen::MatrixXd * spd_out);
	void train(Eigen::MatrixXd * spd_pred, Eigen::MatrixXd * spd_act, Eigen::MatrixXd * spd_in);
	std::vector<std::vector<Eigen::MatrixXd*>*>* getVals();
    void setVals(std::vector<std::vector<Eigen::MatrixXd*>*>* vals);
	int getI();
	int getO();
	void formatInData(Eigen::MatrixXd * input);
	void formatOutData(Eigen::MatrixXd * output);
    void scaleTrainingSpeed(Eigen::MatrixXd * input);
    void unscaleTrainingSpeed(Eigen::MatrixXd * output);
    int getNumLayers();
    float getDS();
    int getMaxSpeed();
    float getSpeedOffset();
    void output2Input(Eigen::MatrixXd* spdIn, Eigen::MatrixXd* spdOut);
};

}

#endif /* SPEED_PREDICTION_SPEEDPREDICTION_H_ */
