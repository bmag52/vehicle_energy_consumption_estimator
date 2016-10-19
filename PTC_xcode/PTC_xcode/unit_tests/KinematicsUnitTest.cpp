/*
 * KinematicsUnitTest.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../kinematics/Kinematics.h"

#include <assert.h>
#include <iostream>
#include <fstream>

using namespace PredictivePowertrain;

void kinematics_ut() {
    
    int length = 1000;
    Kinematics kin;
    std::string num;
    
    std::vector<float> spdUpVec(length);
    std::ifstream spdUp("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/kinSpdIncrease.csv");
    for (int i = 0; i<length; i++){
        std::getline(spdUp, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        spdUpVec.at(i) = f;
    }
    
    std::vector<float> spdDownVec(length);
    std::ifstream spdDown("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/kinSpdDecrease.csv");
    for (int i = 0; i<length; i++){
        std::getline(spdDown, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        spdDownVec.at(i) = f;
    }
    
    std::vector<float> eleUpVec(length);
    std::ifstream eleUp("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/kinEleIncline.csv");
    for (int i = 0; i<length; i++){
        std::getline(eleUp, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        eleUpVec.at(i) = f;
    }

    std::vector<float> eleDownVec(length);
    std::ifstream eleDown("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/kinEleDecline.csv");
    for (int i = 0; i<length; i++){
        std::getline(eleDown, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        eleDownVec.at(i) = f;
    }
    
    std::vector<float> dt(length);
    for(int i = 0; i < length; i++) { dt.at(i) = .5; }
    
    float negTractiveEnergy = kin.runKinematics(spdDownVec, dt, eleDownVec, false);
    assert(negTractiveEnergy < 0.0);
    
    float posTractiveEnergy = kin.runKinematics(spdUpVec, dt, eleUpVec, false);
    assert(posTractiveEnergy > 0.0);
    
}
