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
std::pair<Eigen::MatrixXd*, std::vector<float>*> DriverPrediction::startPrediction(Link* currentLink,
                                                                std::vector<float> currentConditions,
                                                                int distanceIndexAlongLink)
{
    this->currLink = currentLink;
    Route* predRoute = this->rp->startPrediction(this->city->getIntersectionFromLink(currentLink, false), &currentConditions);
    std::vector<float>* predElevData = this->city->routeToElevData(predRoute, distanceIndexAlongLink);
    Eigen::MatrixXd* predSpdData = this->predictSpeed(predRoute);
    
    std::pair<Eigen::MatrixXd*, std::vector<float>*> prediction(predSpdData, predElevData);
    return prediction;
    
    
}

std::pair<Eigen::MatrixXd*, std::vector<float>*> DriverPrediction::nextPrediction(Link* currentLink,
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
        GenericMap<int, Link*>* predictedLinks = this->rp->getPredictedRoute()->getLinks();
        Goal* predictedGoal = this->rp->getPredictedRoute()->getGoal();
        predRoute = new Route(predictedLinks, predictedGoal);
    }
    
    std::vector<float>* predElevData = this->city->routeToElevData(predRoute, distanceIndexAlongLink);
    Eigen::MatrixXd* predSpdData = this->predictSpeed(predRoute);
    
    std::pair<Eigen::MatrixXd*, std::vector<float>*> prediction(predSpdData, predElevData);
    return prediction;
}

void DriverPrediction::parseRoute(Route* currRoute, Eigen::MatrixXd* spd)
{
    
}

int DriverPrediction::getSpdPredictionInterval()
{
    return this->spdPredictionInterval;
}
    
Eigen::MatrixXd* DriverPrediction::predictSpeed(Route* predRoute)
{
    return NULL;
    
}
    
} /* namespace PredictivePowertrain */
