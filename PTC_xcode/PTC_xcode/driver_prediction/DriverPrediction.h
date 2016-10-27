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

#include <queue> 
#include <vector>

namespace PredictivePowertrain {
    
    class DriverPrediction {
    private:
        std::vector<float> linkSpds;
        std::queue<float> lastSpds;
        RoutePrediction* rp;
        SpeedPrediction* sp;
        Link* currLink;
        City* city;
        
        Eigen::MatrixXd getSpeedPredInpunt(float spd);
        void trainSpeedPredictionOverLastLink();
        
    public:
        typedef std::pair<std::vector<float>, std::vector<float>> PredData;
        
        DriverPrediction(City* city);
        PredData startPrediction(Link* currentLink, float spd, std::vector<float> currentConditions, float distAlongLink);
        PredData nextPrediction(Link* currentLink, float spd, float distAlongLink);
        void parseRoute(Route* currRoute);
        
        
    };
    
} /* namespace PredictivePowertrain */

#endif /* DRIVER_PREDICTION_DRIVERPREDICTION_H_ */
