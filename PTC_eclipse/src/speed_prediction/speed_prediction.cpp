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

void speed_prediction::initzialize(double * spd_trc[])
{
	this->alpha = 10.0;
	this->I = 100;
	this->HN = {80, 65, 50, 35, 20};
	this->HL = sizeof(this->HN);
	this->O = 14;
//	this->totalLayers =
}

}

