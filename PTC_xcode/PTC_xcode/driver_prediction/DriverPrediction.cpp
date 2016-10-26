/*
 * DriverPrediction.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Brian
 */

#include "DriverPrediction.h"

namespace PredictivePowertrain {
    

DriverPrediction::DriverPrediction(City* city)
{
    this->city = city;
    this->rp = new RoutePrediction(this->city);
    this->sp = new SpeedPrediction();
}

// assumes vehicle speed is zero
std::pair<std::vector<float>*, std::vector<float>*> DriverPrediction::startPrediction(Link* currentLink,
                                                                Eigen::MatrixXd* spd,
                                                                std::vector<float> currentConditions,
                                                                int distanceIndexAlongLink)
{
    this->currLink = currentLink;
    
    Route* predRoute = this->rp->startPrediction(this->city->getIntersectionFromLink(currentLink, false), &currentConditions);
    
    std::pair<std::vector<float>*, std::vector<float>*> predData = this->city->routeToData(predRoute,
                                                                                           distanceIndexAlongLink,
                                                                                           this->sp,
                                                                                           spd);
    
    return predData;
    
    
}

std::pair<std::vector<float>*, std::vector<float>*> DriverPrediction::nextPrediction(Link* currentLink,
                                                                Eigen::MatrixXd* spd,
                                                                int distanceIndexAlongLink)
{
    Route* predRoute;
    
    // at end of route
    if(currentLink->isFinalLink())
    {
        std::cout << "current link is final link" << std::endl;
    }
    
    // must repredict route
    else if(!this->currLink->isEqual(currLink))
    {
        predRoute = this->rp->predict(currentLink);
        this->currLink = currentLink;
    }
    
    // use existing predicted route
    else
    {
        predRoute = this->rp->getPredictedRoute();
    }
    
    std::pair<std::vector<float>*, std::vector<float>*> predData = this->city->routeToData(predRoute,
                                                                                           distanceIndexAlongLink,
                                                                                           this->sp,
                                                                                           spd);

    return predData;
}

void DriverPrediction::parseRoute(Route* currRoute, Eigen::MatrixXd* spd)
{
    
}


    
} /* namespace PredictivePowertrain */
