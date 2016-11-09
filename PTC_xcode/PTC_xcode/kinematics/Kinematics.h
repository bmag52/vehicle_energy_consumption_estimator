
/*
 * Kinematics.h
 *
 *  Created on: May 5, 2016
 *      Author: vagrant
 */

#ifndef OPTIMIZER_OPTIMIZER500_H_
#define OPTIMIZER_OPTIMIZER500_H_

#include <math.h>
#include <vector>
#include <utility>
#include <cmath>

namespace PredictivePowertrain {
    
    class Kinematics {
    private:
        static int idx;
        int distLookAhead;
        int distInterval;
        int vehicleMass;
        double airDensity;
        double frontSurfaceArea;
        double dragCoefficient;
        double rollCoefficient;
        double gravityAcceleration;
        
        bool trqCut;
        bool regen;
        float tractiveEnergy;
    public:
        Kinematics();
        float runKinematics(std::vector<float> predictedSpeed,
                            float ds,
                            std::vector<float> predictedElevation,
                            bool predChange);
    };
}



#endif /* OPTIMIZER_OPTIMIZER500_H_ */
