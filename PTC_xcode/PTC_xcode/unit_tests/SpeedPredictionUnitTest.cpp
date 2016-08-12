//
//  SpeedPredictionUnitTest.cpp
//  PTC_xcode
//
//  Created by Brian on 8/7/16.
//  Copyright © 2016 Brian. All rights reserved.
//

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
    
    int testEpochs = 100; // number of calls
    int refreshRate = 1; // call every one unit of input
    
    SpeedPrediction sp;
    
    std::ofstream myfile("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/speedPredictionResults.csv");
    std::ifstream input("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/spd.csv");
    int I = sp.getI();
    int O = sp.getO();
    
    Eigen::MatrixXd spd_in = Eigen::MatrixXd::Zero(1,10000);
    Eigen::MatrixXd spd_temp = Eigen::MatrixXd::Random(1,10000)*150;
    Eigen::MatrixXd spd_act = Eigen::MatrixXd::Zero(1, 9986);
    Eigen::MatrixXd spd_pred = Eigen::MatrixXd::Zero(1, 9986);
    
    string num;
    
    // read in speed from csv
    for (int i = 0; i<10000; i++){
        getline(input, num, ',');
        stringstream fs(num);
        double f = 0.0;
        fs >> f;
        spd_in(0, i) = f;
    }
    
    int k = I;
    
    // k < 9985 (original)
    while ( k < I + testEpochs){
        Eigen::MatrixXd temp_in = Eigen::MatrixXd::Ones(1, I+1);
        memcpy(&temp_in(0,0), &spd_in(0, k-I), I*sizeof(double));
        
        Eigen::MatrixXd temp_act = Eigen::MatrixXd::Zero(1,O);
        memcpy(&temp_act(0,0), &spd_in(0,k), O*sizeof(double));
        
        Eigen::MatrixXd temp_pred = Eigen::MatrixXd::Zero(1,O);
        
        // Format input data
        sp.formatInData(&temp_in);
        sp.scaleTrainingSpeed(&temp_act);
        
        // predict with historical data
        sp.predict(&temp_in, &temp_pred);
        
        // train the model
        sp.train(&temp_pred, &temp_act, &temp_in);
        
        // format output data
        sp.formatOutData(&temp_pred);
        sp.unscaleTrainingSpeed(&temp_act);
        
        // concatenate predicted and actual data
        int index = k-(I+1);
        for (int j =0; j < O; j++){
            spd_pred(0, index+j) = temp_pred(0, j);
            spd_act(0, index+j) = temp_act(0, j);
        }
        
        // update k
        k = k + refreshRate;
        
        std::cout<< k << std::endl;
    }
    
    // write the actual and predicted speed
    for (int i = 0; i < testEpochs; i++){
        myfile << spd_act(0,i);
        myfile << ", ";
        myfile << spd_pred(0,i);
        myfile << "\n";
    }
}
