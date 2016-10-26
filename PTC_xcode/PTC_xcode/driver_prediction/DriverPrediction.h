/*
 * DriverPrediction.h
 *
 *  Created on: Jan 5, 2016
 *      Author: Brian
 */

#ifndef DRIVER_PREDICTION_DRIVERPREDICTION_H_
#define DRIVER_PREDICTION_DRIVERPREDICTION_H_

#include "../route_prediction/RoutePrediction.h"
#include "../speed_prediction/SpeedPrediction.h"
#include "../city/City.h"
#include "Link.h"

namespace PredictivePowertrain {
    
    class DriverPrediction {
    private:
        RoutePrediction* rp;
        SpeedPrediction* sp;
        Link* currLink;
        City* city;
        
    public:
        DriverPrediction(City* city);
        
        std::pair<std::vector<float>*, std::vector<float>*> startPrediction(Link* currentLink,
                                                                        Eigen::MatrixXd* spd,
                                                                        std::vector<float> currentConditions,
                                                                        int distanceIndexAlongLink);
        
        std::pair<std::vector<float>*, std::vector<float>*> nextPrediction(Link* currentLink,
                                                                        Eigen::MatrixXd* spd,
                                                                        int distanceIndexAlongLink);
        
        void parseRoute(Route* currRoute, Eigen::MatrixXd* spd);
        
        
    };
    
} /* namespace PredictivePowertrain */

#endif /* DRIVER_PREDICTION_DRIVERPREDICTION_H_ */
