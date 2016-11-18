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

    this->intitalize();
}
    

DriverPrediction::DriverPrediction()
{
    this->intitalize();
}
    
void DriverPrediction::intitalize()
{
    this->sp = new SpeedPrediction();
}
    
DriverPrediction::~DriverPrediction()
{
    delete(this->sp);
}
    
void DriverPrediction::setCurrentLink(Link* currentLink)
{
this->currLink = currentLink;
}

// assumes vehicle speed is zero
DriverPrediction::PredData DriverPrediction::startPrediction(Link* currentLink,
                                                                float spd,
                                                                std::vector<float>* currentConditions,
                                                                float distAlongLink)
{
    // update current link
    this->currLink = currentLink;
    
    // prep for route and speed predictions across route
    Intersection* startIntersection = this->city->getIntersectionFromLink(currentLink, false);
    this->rp->startPrediction(startIntersection, currentConditions);
    
    // get predicted route and add currentLink to start of route
    Route* predRoute = this->rp->predict(currentLink)->copy();
    
    // update current route
    this->predRoute = predRoute;
    
    PredData predData;
    predData.first.push_back(-1);
    predData.first.push_back(-1);
    if(!predRoute->isEqual(this->rp->getUnknownRoute()) && !predRoute->isEqual(this->rp->getOverRoute()))
    {
        predRoute->addLinkToFront(currentLink);
        
        // add NN values to route since route prediction always returns old links
        this->addWeightedLinksToRoute(predRoute);
        
        // get route speed and elevation data
        Eigen::MatrixXd spdIn = this->getSpeedPredInpunt(spd);
        predData = this->city->routeToData(this->predRoute, distAlongLink, this->sp, &spdIn);
    }
    
    return predData;
}

DriverPrediction::PredData DriverPrediction::nextPrediction(Link* currentLink,
                                                                float spd,
                                                                float distAlongLink)
{
    // --- AT END OF PREDICTION ---
    if(currentLink->isFinalLink())
    {
        std::cout << "current link is final link" << std::endl;
    }
    
    // --- REPREDICT ROUTE AND TRAIN SPEED PREDICTION ---
    else if(!this->currLink->isEqual(currentLink))
    {
        if(!predRoute->isEqual(this->rp->getUnknownRoute()) && !predRoute->isEqual(this->rp->getOverRoute()))
        {
            // quick train of speed prediction over last link
            this->trainSpeedPredictionOverLastLink();
        }
        // perform route prediction
        Route* newPredRoute = this->rp->predict(currentLink)->copy();
        
        // add speed prediction vals to route and attached current link to front
        if(!newPredRoute->isEqual(this->rp->getUnknownRoute()) && !newPredRoute->isEqual(this->rp->getOverRoute()))
        {
            newPredRoute->addLinkToFront(currentLink);
            this->addWeightedLinksToRoute(newPredRoute);
        }
        
        // update current link and predicted route
        this->currLink = currentLink;
        delete(this->predRoute);
        this->predRoute = newPredRoute;
    }
    
    PredData predData;
    predData.first.push_back(-1);
    predData.first.push_back(-1);
    if(!this->predRoute->isEqual(this->rp->getUnknownRoute()) && !this->predRoute->isEqual(this->rp->getOverRoute()))
    {
        Eigen::MatrixXd spdIn = this->getSpeedPredInpunt(spd);
        predData = this->city->routeToData(this->predRoute, distAlongLink, this->sp, &spdIn);
    }

    return predData;
}

void DriverPrediction::parseRoute(Route* currRoute, std::vector<float>* spds)
{
    // train routpe prediction
    this->rp->parseRoute(currRoute);
    
    // train speed prediction
    this->beforeLinkSpds.clear();
    this->linkSpds.clear();
    std::queue<float> empty;
    std::swap(this->lastSpds, empty);
    
    std::vector<float> spdsCopy(*spds);
    float spdDist = spds->size() * this->sp->getDS();
    
    currRoute->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = currRoute->getLinks()->nextEntry();
    while(nextLink != NULL && !nextLink->value->isFinalLink())
    {
        // get necessary road data
        Road* road_i = this->city->getRoads()->getEntry(nextLink->value->getNumber());
        float roadDist_i = road_i->getSplineLength();
        float roadSpdIndices_i = (float) roadDist_i / spdDist * spds->size();
        
        std::vector<float> roadSpds_i;
        
        // get road speeds from arbitrary trace of continuous speed values.
        for(int i = 0; i < roadSpdIndices_i; i++)
        {
            roadSpds_i.push_back(spdsCopy.front());
            spdsCopy.erase(spdsCopy.begin());
        }
        
        // train for speed at beginnging of next link
        roadSpds_i.push_back(spdsCopy.at(0));
        
        // set current link
        this->setCurrentLink(nextLink->value);
        
        // populate link speed buffers
        while(roadSpds_i.size() > 0)
        {
            this->updateSpeedsbyVal(roadSpds_i.front());
            roadSpds_i.erase(roadSpds_i.begin());
        }
        
        // train
        this->trainSpeedPredictionOverLastLink();
        
        nextLink = currRoute->getLinks()->nextEntry();
    }
    delete(nextLink);

}
    
void DriverPrediction::trainSpeedPredictionOverLastLink()
{
    int trainIters = 10;
    
    // matrices for speed prediction
    Eigen::MatrixXd spdIn(1,this->sp->getI()+1);
    Eigen::MatrixXd spdOut(1,this->sp->getO());
    Eigen::MatrixXd spdAct(1,this->sp->getO());
    
    // get spd input from before line and reset before speed input
    for(int i = 0; i < spdIn.cols(); i++)
    {
        spdIn.coeffRef(0,i) = this->beforeLinkSpds.at(i);
        
        float spd_i = this->lastSpds.front();
        this->beforeLinkSpds.at(i) = spd_i;
        
        this->lastSpds.pop();
        this->lastSpds.push(spd_i);
    }
    
    // prep speed prediction for training
    if(this->currLink->linkHasWeights())
    {
        std::vector<std::vector<Eigen::MatrixXd*>*>* nnVals = this->currLink->getWeights(this->currLink->getDirection());
        this->sp->setVals(nnVals);
    }
    
    // consume link speed values to train NN
    for(int i = 0; i < spdAct.cols(); i++)
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
            spdAct.coeffRef(0,i) = 0.0;
        }
    }
    
    // scale training data
    this->sp->formatInData(&spdIn);
    this->sp->scaleTrainingSpeed(&spdAct);
    
    // train
    for(int i = 0; i < trainIters; i++)
    {
        this->sp->predict(&spdIn, &spdOut);
        this->sp->train(&spdOut, &spdAct, &spdIn);
    }
        
    // perform rolling window trainging
    while(this->linkSpds.size() > 0)
    {
        // left shift speed input
        for(int i = 0; i < spdIn.cols(); i++)
        {
            spdIn.coeffRef(0,i) = spdIn.coeffRef(0,i+1);
        }
        spdIn.coeffRef(0, this->sp->getI()) = spdAct.coeffRef(0, 0);
        
        // left shift speed act values
        for(int i = 0; i < spdAct.cols() - 1; i++)
        {
            spdAct.coeffRef(0, i) = spdAct.coeffRef(0,i+1);
        }
        spdAct.coeffRef(0, spdAct.cols() - 1) = this->linkSpds.at(0) / sp->getMaxSpeed() + sp->getSpeedOffset();
        
        // remove first value from link speed
        this->linkSpds.erase(this->linkSpds.begin());
        
        // train
        for(int i = 0; i < trainIters; i++)
        {
            this->sp->predict(&spdIn, &spdOut);
            this->sp->train(&spdOut, &spdAct, &spdIn);
        }
    }
    
    // update link weights
    std::vector<std::vector<Eigen::MatrixXd*>*>* spVals = this->sp->getVals();
    this->currLink->setWeights(spVals->at(0), spVals->at(1), spVals->at(2), this->currLink->getDirection());
}
    
void DriverPrediction::updateSpeedsByVec(std::vector<float>* spds)
{
    while(spds->size() > 0)
    {
        this->updateSpeedsbyVal(spds->front());
        spds->erase(spds->begin());
    }
}

void DriverPrediction::updateSpeedsbyVal(float spd)
{
    // because before links speeds doesn't exist on startup, use repeated first measurement.
    if(this->beforeLinkSpds.size() == 0)
    {
        for(int i = 0; i < this->sp->getI() + 1; i++)
        {
            this->beforeLinkSpds.push_back(spd);
        }
    }
    
    // update link spd trace for training later
    this->linkSpds.push_back(spd);
    
    // update speed FIFO queue
    this->lastSpds.push(spd);
    if(this->lastSpds.size() > this->sp->getI() + 1)
    {
        this->lastSpds.pop();
    }
}
    
Eigen::MatrixXd DriverPrediction::getSpeedPredInpunt(float spd)
{
    // update speeds
    this->updateSpeedsbyVal(spd);

    // create a matrix of zero input
    Eigen::MatrixXd spdIn = Eigen::MatrixXd::Zero(1, this->sp->getI() + 1);
    
    // take most recent speed values from link speed and populate end of speed pred input
    for(size_t i = 0; i < spdIn.cols(); i++)
    {
        float spd_i;
        
        if(i < this->lastSpds.size())
        {
            spd_i = this->lastSpds.front();
            this->lastSpds.pop();
            this->lastSpds.push(spd_i);
        }
        else
        {
            spd_i = this->lastSpds.back();
        }
        
        spdIn.coeffRef(0, i) = spd_i;
    }
    
    return spdIn;
}
    
void DriverPrediction::addWeightedLinksToRoute(Route* unweightedRoute)
{
    GenericMap<long int, Link*>* weightedRouteLinks = new GenericMap<long int, Link*>();
    GenericMap<long int, Link*>* unweightedLinks = unweightedRoute->getLinks();
    
    unweightedLinks->initializeCounter();
    GenericEntry<long int, Link*>* nextUnweightedLink = unweightedLinks->nextEntry();
    while(nextUnweightedLink != NULL)
    {
        long int key = nextUnweightedLink->key;
        Link* nextUnweightedLink_i = nextUnweightedLink->value;
        
        weightedRouteLinks->addEntry(key, this->rp->getLinks()->getEntry(nextUnweightedLink_i->getHash()));
        nextUnweightedLink = unweightedLinks->nextEntry();
    }
    delete(nextUnweightedLink);
    
    unweightedRoute->replaceLinks(weightedRouteLinks);
}

RoutePrediction* DriverPrediction::getRP()
{
    return this->rp;
}

SpeedPrediction* DriverPrediction::getSP()
{
    return this->sp;
}

    
} /* namespace PredictivePowertrain */
