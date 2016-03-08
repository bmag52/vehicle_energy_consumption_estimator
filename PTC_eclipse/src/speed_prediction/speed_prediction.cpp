#include <Eigen/Dense>

using Eigen::MatrixXd;

#include "speed_prediction.h"

namespace speed_prediction {

speed_prediction::speed_prediction()
{
	init_params();

	// initialize weights of NN
	this->Wts = new Eigen::MatrixXd[this->HL+2];
	for(int i = 0; i < this->HL + 1; i++)
	{
		int L1 = this->totalLayers[i];
		int L2 = this->totalLayers[i+1];

		this->Wts[i] = Eigen::MatrixXd::Zero(L2,L1);

		for(int j = 0; j <= L1; j++)
		{
			for(int k = 0; k < L2; k++)
			{
				this->Wts[i](k,j) = -.01 + .02 * (rand() % 10) / 10;
			}
		}
	}

	// initialize hidden layer outputs
	this->yHid = Eigen::MatrixXd::Zero(this->HL,this->HN[0]+1);

	// initialize hidden layer inputs
	this->yInHid = Eigen::MatrixXd::Zero(this->HL+1,this->HN[0]);
}

speed_prediction::speed_prediction(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid)
{
	init_params();

	this->Wts = Wts;
	this->yHid = *yHid;
	this->yInHid = *yInHid;
}

void speed_prediction::init_params()
{
	// NN architectural params
	this->alpha = 10.0;					// learning rate
	this->I = 100;						// input neurons
	this->O = 14;						// output neurons
	int HN[] = {80, 65, 50, 35, 20};	// hidden layer neurons

	// init hidden layers
	this->HL = sizeof(HN)/4;
	this->HN = new int[this->HL];
	for(int i = 0; i < this->HL; i++)
	{
		this->HN[i] = HN[i];
	}

	// init total layers
	this->totalLayers = new int[this->HL+2];
	this->totalLayers[0] = I;
	for(int i = 1; i <= HL; i++)
	{
		this->totalLayers[i] = this->HN[i-1];
	}
	this->totalLayers[this->HL+1] = O;
}

// feed-forward prediction
void speed_prediction::predict(double * spd_in, double * spd_out)
{

}

// back-propagation weight retraining
void speed_prediction::train(double * spd_pred, double * spd_act)
{

}

// send address of weights
// TODO have a copy_wts function
void speed_prediction::get_vals(Eigen::MatrixXd * Wts, Eigen::MatrixXd * yHid, Eigen::MatrixXd * yInHid)
{
	Wts = this->Wts;
	yHid = &this->yHid;
	yInHid = &this->yInHid;
}

}

