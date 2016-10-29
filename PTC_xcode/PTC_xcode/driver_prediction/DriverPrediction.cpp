/*
 * DriverPrediction.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Brian
 */

#include "DriverPrediction.h"

namespace PredictivePowertrain {
    

DriverPrediction::DriverPrediction(RoutePrediction* newRP)
{
    this->city = newRP->getCity();
    this->rp = newRP;
    this->sp = new SpeedPrediction();
    
    for(int i = 0; i < this->sp->getI()+1; i++)
    {
        this->linkSpds.push_back(0.0);
        this->lastSpds.push(0.0);
    }
}

// assumes vehicle speed is zero
DriverPrediction::PredData DriverPrediction::startPrediction(Link* currentLink,
                                                                float spd,
                                                                std::vector<float>* currentConditions,
                                                                float distAlongLink)
{
    this->currLink = currentLink;
    
    Route* predRoute = this->rp->startPrediction(this->city->getIntersectionFromLink(currentLink, false), currentConditions);
    
    Eigen::MatrixXd spdIn = this->getSpeedPredInpunt(spd);
    PredData predData = this->city->routeToData(predRoute, distAlongLink, this->sp, &spdIn);
    
    return predData;
    
    
}

DriverPrediction::PredData DriverPrediction::nextPrediction(Link* currentLink,
                                                                float spd,
                                                                float distAlongLink)
{
    Route* predRoute;
    
    // --- AT END OF PREDICTION ---
    if(currentLink->isFinalLink())
    {
        std::cout << "current link is final link" << std::endl;
    }
    
    // --- REPREDICT ROUTE AND TRAIN SPEED PREDICTION ---
    else if(!this->currLink->isEqual(currentLink))
    {
        // quick train of speed prediction over last link
        this->trainSpeedPredictionOverLastLink();
        
        // perform route prediction
        predRoute = this->rp->predict(currentLink);
        this->currLink = currentLink;
    }
    
    // --- USE PREDICTED ROUTE FOR DATA EXTRACTION ---
    else
    {
        predRoute = this->rp->getPredictedRoute();
    }
    
    Eigen::MatrixXd spdIn = this->getSpeedPredInpunt(spd);
    PredData predData = this->city->routeToData(predRoute, distAlongLink, this->sp, &spdIn);

    return predData;
}

void DriverPrediction::parseRoute(Route* currRoute)
{
    
}
    
void DriverPrediction::trainSpeedPredictionOverLastLink()
{
    Eigen::MatrixXd spdIn(1,this->sp->getI()+1);
    Eigen::MatrixXd spdOut(1,this->sp->getO());
    Eigen::MatrixXd spdAct(1,this->sp->getO());
    
    // get spd input from first speed logged link speed values
    for(int i = 0; i < this->sp->getI()+1; i++)
    {
        spdIn.coeffRef(0,i) = this->linkSpds.at(i);
        this->linkSpds.erase(this->linkSpds.begin());
    }
    
    // prep speed prediction for training
    this->sp->setVals(this->currLink->getWeights(this->currLink->getDirection()));
    this->sp->predict(&spdIn, &spdOut);
    
    // get actual speed for training from logged link speed
    for(int i = 0; i < this->sp->getO(); i++)
    {
        // use link speed logs to set actual speed values while log is large enough
        if(this->linkSpds.size() > 0)
        {
            spdAct.coeffRef(0,i) = this->linkSpds.at(0);
            this->linkSpds.erase(this->linkSpds.begin());
            
        }
        
        // otherwise just use the NN output as act and do not penalize NN for wrong predictions
        else
        {
            spdAct.coeffRef(0,i) = spdOut.coeffRef(0,i);
        }
    }
    
    // perform rolling window speed prediction training
    this->sp->train(&spdOut, &spdAct, &spdIn);
    while(this->linkSpds.size() > 0)
    {
        // left shift speed input
        for(int i = 0; i < this->sp->getI(); i++)
        {
            spdIn.coeffRef(0,i) = spdIn.coeffRef(0,i+1);
        }
        spdIn.coeffRef(0, this->sp->getI()) = spdAct.coeffRef(0, 0);
        
        // left shift speed act values
        for(int i = 0; i < this->sp->getO() - 1; i++)
        {
            spdAct.coeffRef(0, i) = spdAct.coeffRef(0,i+1);
        }
        spdAct.coeffRef(0, this->sp->getO() - 1) = this->linkSpds.at(0);
        
        // remove first value from link speed
        this->linkSpds.erase(this->linkSpds.begin());
        
        // rolling window train
        this->sp->predict(&spdIn, &spdOut);
        this->sp->train(&spdOut, &spdAct, &spdIn);
    }
    
    // update link weights
    std::vector<std::vector<Eigen::MatrixXd*>*>* spVals = this->sp->getVals();
    this->currLink->setWeights(spVals->at(0), spVals->at(1), spVals->at(2), this->currLink->getDirection());
    
    // reset link speed log
    for(int i = 0; i < this->sp->getI() + 1; i++)
    {
        float spd_i = this->lastSpds.front();
        this->lastSpds.pop();
        
        this->linkSpds.push_back(spd_i);
        this->lastSpds.push(spd_i);
    }
}
    
Eigen::MatrixXd DriverPrediction::getSpeedPredInpunt(float spd)
{
    // update link spd trace for training later
    this->linkSpds.push_back(spd);
    
    // update speed FIFO queue
    this->lastSpds.push(spd);
    this->lastSpds.pop();

    
    // create a matrix of zero input
    Eigen::MatrixXd spdIn = Eigen::MatrixXd::Zero(1, this->sp->getI() + 1);
    
    // take most recent speed values from link speed and populate end of speed pred input
    for(size_t i = 0; i < spdIn.cols(); i++)
    {
        float spd_i = this->lastSpds.front();
        this->lastSpds.pop();
        
        spdIn.coeffRef(0, i) = spd_i;
        this->lastSpds.push(spd_i);
    }
    
    return spdIn;
}


    
} /* namespace PredictivePowertrain */
