/*
 * speed_prediction.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef SPEED_PREDICTION_SPEED_PREDICTION_H_
#define SPEED_PREDICTION_SPEED_PREDICTION_H_

namespace speed_prediction {

class speed_prediction {
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

	void init_params();
	void print_all();
public:
	speed_prediction();
	speed_prediction(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
	void predict(Eigen::MatrixXd * spd_in, Eigen::MatrixXd * spd_out);
	void train(Eigen::MatrixXd * spd_pred, Eigen::MatrixXd * spd_act, Eigen::MatrixXd * spd_in);
	void get_vals(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid);
	int get_I();
	int get_O();
	void format_inData(Eigen::MatrixXd * input);
	void format_outData(Eigen::MatrixXd * output);
};

}

#endif /* SPEED_PREDICTION_SPEED_PREDICTION_H_ */
