#include <Eigen/Dense>
#include "speed_prediction.h"

using Eigen::MatrixXd;

namespace speed_prediction {

speed_prediction::speed_prediction(double * spd_trc[])
{

}

speed_prediction::speed_prediction(double * spd_trc[], Eigen::MatrixXd * Wts)
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

