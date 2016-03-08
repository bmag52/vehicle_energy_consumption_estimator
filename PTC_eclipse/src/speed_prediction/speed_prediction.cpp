#include <Eigen/Dense>

using Eigen::MatrixXd;
#include "speed_prediction.h"

namespace speed_prediction {

speed_prediction::speed_prediction()
{

}

speed_prediction::speed_prediction(Eigen::MatrixXd * Wts)
{

}

void speed_prediction::init_params()
{

	// init HN
	int HN[] = {80, 65, 50, 35, 20};
	this->HN = HN;

	// init input / output dimensions
	this->alpha = 10.0;
	this->I = 100;
	this->HL = sizeof(this->HN);
	this->O = 14;

	// init total layers
	int totalLayers[this->HL+2] = { 0 };
	this->totalLayers = totalLayers;
	this->totalLayers[0] = I;
	for(int i = 1; i < HL; i++)
	{
		this->totalLayers[i] = this->HN[i-1];
	}
	this->totalLayers[this->HL+1] = O;
}


}

