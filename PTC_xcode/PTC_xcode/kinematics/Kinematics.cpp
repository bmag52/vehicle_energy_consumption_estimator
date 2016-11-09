/*
 * Kinematics.cpp
 *
 *  Created on: May 5, 2016
 *      Author: vagrant
 */


#include "Kinematics.h"


namespace PredictivePowertrain {
    
Kinematics::Kinematics()
{
    this->distLookAhead = 28;
    this->distInterval = 5;
    this->vehicleMass = 1508.195;
    this->airDensity = 0.3;
    this->frontSurfaceArea = 3;
    this->dragCoefficient = .3;
    this->rollCoefficient = 0.06;
    this->gravityAcceleration = 9.81;
}

float Kinematics::runKinematics(std::vector<float> predictedSpeed,
                                float ds,
                                std::vector<float> predictedElevation,
                                bool predChange)
{
    // calculate tractive energy usage over look ahead distance
    float tractiveEnergy = 0.0;
    for (int i = 0; i < this->distLookAhead; i++)
    {
        // acceleration
        float va = predictedSpeed.at(i);
        float vb = predictedSpeed.at(i + 1);
        float vehicleAcceleration_i = (std::pow(vb, 2) - std::pow(va, 2)) / (2 * ds);
        
        // elevation angle
        float ea = predictedElevation.at(i);
        float eb = predictedElevation.at(i + 1);
        float elevationAngle_i = std::atan2(eb - ea, this->distInterval);
        
        // tractive force
        float intertialForce = vehicleMass * vehicleAcceleration_i + vehicleMass * gravityAcceleration * std::sin(elevationAngle_i);
        float dragForce = 0.5 * airDensity * frontSurfaceArea * dragCoefficient * std::pow(predictedSpeed.at(i), 2);
        float rollingForce = rollCoefficient * vehicleMass * gravityAcceleration * std::cos(elevationAngle_i);
        float tractiveForce = intertialForce - dragForce - rollingForce;
                                
        // tractive energy
        tractiveEnergy += tractiveForce * this->distInterval;
    }

    return tractiveEnergy;
}
    
}
