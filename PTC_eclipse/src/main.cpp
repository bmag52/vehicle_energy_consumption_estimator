/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <iostream>
#include <Eigen/Dense>

#include "speed_prediction/speed_prediction.h"

using namespace std;
using namespace speed_prediction;
using Eigen::MatrixXd;

int main() {

	speed_prediction::speed_prediction sp;

	int I = sp.get_I();
	int O = sp.get_O();

	Eigen::MatrixXd spd_in = Eigen::MatrixXd::Random(1,I+1)*150;
	Eigen::MatrixXd spd_act = Eigen::MatrixXd::Random(1,O);
	Eigen::MatrixXd spd_pred = Eigen::MatrixXd::Zero(1,O);

	std::cout << spd_in << std::endl;
	sp.format_inData(&spd_in);
	std::cout << spd_in << std::endl;

	sp.predict(&spd_in, &spd_pred);
	sp.train(&spd_pred, &spd_act, &spd_in);

	std::cout << spd_pred << std::endl;
	sp.format_outData(&spd_pred);
	std::cout << spd_pred << std::endl;

	return 0;
}

