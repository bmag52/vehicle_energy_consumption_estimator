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
#include "../gps/GPS.h"

#include <queue> 
#include <vector>

namespace PredictivePowertrain {
    
    class DriverPrediction {
    private:
        std::vector<float> beforeLinkSpds;
        std::vector<float> linkSpds;
        std::queue<float> lastSpds;
        RoutePrediction* rp;
        SpeedPrediction* sp;
        Route* predRoute;
        Link* currLink;
        City* city;
        
        void addWeightedLinksToRoute(Route* unweightedRoute);
        bool allLinksHaveWeigths(Route* route);
        Route* copyRoute(Route* route);
        void intitalize();
        
    public:
        typedef std::pair<std::vector<float>, std::vector<float>> PredData;
        
        virtual ~DriverPrediction();
        DriverPrediction(RoutePrediction* newRP);
        DriverPrediction();
        void setCurrentLink(Link* currentLink);
        PredData startPrediction(Link* currentLink, float spd, std::vector<float>* currentConditions, float distAlongLink);
        PredData nextPrediction(Link* currentLink, float spd, float distAlongLink);
        void parseRoute(Route* currRoute, std::vector<float>* speeds, GenericMap<long int, std::pair<double, double>*>* trace);
        RoutePrediction* getRP();
        SpeedPrediction* getSP();
        Eigen::MatrixXd getSpeedPredInpunt(float spd);
        void trainSpeedPredictionOverLastLink();
        void updateSpeedsByVec(std::vector<float>* spds);
        void updateSpeedsbyVal(float spd);
        std::vector<long int> getRouteDataLabels();
        Route* getPredRoute();
        
    };
    
} /* namespace PredictivePowertrain */

#endif /* DRIVER_PREDICTION_DRIVERPREDICTION_H_ */
