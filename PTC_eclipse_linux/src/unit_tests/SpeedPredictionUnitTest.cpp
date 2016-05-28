/*
 * SpeedPredictionUnitTest.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: vagrant
 */



#include "../speed_prediction/SpeedPrediction.h"
#include "../map/GenericMap.h"
#include "UnitTests.h"

#include <iostream>
#include <assert.h>
#include <fstream>
#include <math.h>


#define PI 3.14159265


using namespace std;
using namespace PredictivePowertrain;


// unit test for the SpeedPrediction class
void speedPrediction_ut(){
	std::cout << "Speed Prediction Unit Test" << std::endl;

	int i = 0;
	while(i < 1000000)
	{
		int ii;
		while(ii < 1000000)
		{
			ii++;
		}
		i++;
	}

//	SpeedPrediction sp;
//
//	std::ofstream myfile("speedPredictionResults.csv");
//	std::ifstream input("testDataSet.csv");
//	int I = sp.getI();
//	int O = sp.getO();
//
//	Eigen::MatrixXd spd_in = Eigen::MatrixXd::Zero(1,10000);
//	Eigen::MatrixXd spd_temp = Eigen::MatrixXd::Random(1,10000)*150;
//	Eigen::MatrixXd spd_act = Eigen::MatrixXd::Zero(1, 9986);
//	Eigen::MatrixXd spd_pred = Eigen::MatrixXd::Zero(1, 9986);
//
//	string num;
//
//	for (int i = 0; i<10000; i++){
//		getline(input, num, ',');
//		stringstream fs(num);
//		double f = 0.0;
//		fs >> f;
//		spd_in(0, i) = f;
//
//	}
//
//	int k = 100;
//
//	// k < 9985 (original)
//	while ( k < 9985){
//		Eigen::MatrixXd temp_in = Eigen::MatrixXd::Ones(1, 101);
//		memcpy(&temp_in(0,0), &spd_in(0, k-100), 100*sizeof(double));
//
//		Eigen::MatrixXd temp_act = Eigen::MatrixXd::Zero(1,O);
//		memcpy(&temp_act(0,0), &spd_in(0,k), O*sizeof(double));
//
//		Eigen::MatrixXd temp_pred = Eigen::MatrixXd::Zero(1,O);
//
//		std::cout<< temp_in<< std::endl;
//		std::cout<<"Actual" << std::endl;
//		std::cout<< temp_act << std::endl;
//
//		// Format input data
//		sp.formatInData(&temp_in);
//
//		// predict with historical data
//		sp.predict(&temp_in, &temp_pred);
//
//		sp.formatOutData(&temp_pred);
//
//		// train the model
//		sp.train(&temp_pred, &temp_act, &temp_in);
//
//		std::cout<< "Predicted" << std::endl;
//		std::cout<< temp_pred << std::endl;
//
//		// add temp_pred data to spd_pred
//		int index = k-101;
//		for (int j =0; j < O; j++){
//			spd_pred(0, index+j) = temp_pred(0, j);
//
//		}
//
//		// add temp_act data to spd_act
//		for (int j = 0; j < O; j++){
//				spd_act(0, index+j) = temp_act(0, j);
//		}
//
//		k = k + O;
//
//		std::cout<< k << std::endl;
//	}
//
//	myfile << "Actual_Speed,";
//	for (int i = 0; i < 9898; i++){
//		myfile << spd_act(0,i);
//		myfile << ", ";
//	}
//
//	/*for (int i =0; i<100; i++){
//		Eigen::MatrixXd temp_in = Eigen::MatrixXd::Zero(1, 101);
//		memcpy(&temp_in(0,0), &spd_in(0, 0)+i*O, 101*sizeof(double));
//		Eigen::MatrixXd temp_act = Eigen::MatrixXd::Zero(1,O);
//		memcpy(&temp_act(0,0), &spd_act(0,0)+i*O, O*sizeof(double));
//		Eigen::MatrixXd temp_pred = Eigen::MatrixXd::Zero(1,O);
//		std::cout<< temp_in<< std::endl;
//		std::cout<<"Actual" << std::endl;
//		std::cout<< temp_act << std::endl;
//		sp.formatInData(&temp_in);
//		sp.predict(&temp_in, &temp_pred);
//		sp.train(&temp_pred, &temp_act, &temp_in);
//		sp.formatOutData(&temp_pred);
//		int index = i * O;
//		for (int j =0; j < O; j++){
//			spd_pred(0, index+j) = temp_pred(0, j);
//		}
//	}*/
//
//	myfile << "\n";
//	myfile << "PredictedSpeed,";
//
//	for (int i = 0; i < 9898; i++){
//		myfile << spd_pred(0, i);
//		myfile << ", ";
//	}
//	myfile << "\n";


}





