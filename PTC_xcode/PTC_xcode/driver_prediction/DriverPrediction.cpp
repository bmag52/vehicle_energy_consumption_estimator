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
    
    // add current link to route
    this->rp->getCurrentRoute()->addLink(currLink);
    
    // update current route
    this->predRoute = predRoute;
    
    PredData predData;
    predData.first.push_back(-1);
    predData.first.push_back(-1);
    if(!predRoute->isEqual(this->rp->getUnknownRoute())
       && !predRoute->isEqual(this->rp->getOverRoute())
       && this->allLinksHaveWeigths(predRoute))
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
    // current link
    std::cout << "current link: " << this->currLink->getNumber() << std::endl;
    
    // print current predicted route
    std::cout << "predicted route: " << std::endl;
    this->predRoute->printLinks();
    
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
    if(!this->predRoute->isEqual(this->rp->getUnknownRoute())
       && !this->predRoute->isEqual(this->rp->getOverRoute())
       && this->allLinksHaveWeigths(this->predRoute))
    {
        Eigen::MatrixXd spdIn = this->getSpeedPredInpunt(spd);
        predData = this->city->routeToData(this->predRoute, distAlongLink, this->sp, &spdIn);
    }

    return predData;
}

void DriverPrediction::parseRoute(Route* currRoute, std::vector<float>* spds, GenericMap<long int, std::pair<double, double>*>* trace)
{
    std::cout << "in driver prediction: parsing route" << std::endl;
    
    // train speed prediction
    GPS gps;
    
    // clear out dp buffers since no longer training NN on the fly
    this->beforeLinkSpds.clear();
    this->linkSpds.clear();
    std::queue<float> empty;
    std::swap(this->lastSpds, empty);
    
    // make copy of actual speed values since this will be mutilated
    std::vector<float> spdsCopy(*spds);
    
    // trim off actual speed measurements taken over first link
    Link* firstRouteLink = currRoute->getLinks()->getEntry(0);
    
    // erase from front of speed trace
    Road* road_i = this->city->getRoads()->getEntry(firstRouteLink->getNumber());
    
    road_i->getNodes()->initializeCounter();
    GenericEntry<long int, Node*>* nextNode = road_i->getNodes()->nextEntry();
    while(nextNode != NULL)
    {
        trace->initializeCounter();
        GenericEntry<long int, std::pair<double, double>*>* nextMeas = trace->nextEntry();
        while(nextMeas != NULL)
        {
            double nodeLat = nextNode->value->getLat();
            double nodeLon = nextNode->value->getLon();
            
            double traceLat = nextMeas->value->first;
            double traceLon = nextMeas->value->second;
            
            float dist = gps.deltaLatLonToXY(nodeLat, nodeLon, traceLat, traceLon);
            
            if(dist < gps.getDeltaXYTolerance())
            {
                spdsCopy.erase(spdsCopy.begin());
                break;
            }
            
            nextMeas = trace->nextEntry();
        }
        delete(nextMeas);
        
        nextNode = road_i->getNodes()->nextEntry();
    }
    delete(nextNode);
    
    // get distance covered from trimmed actual speed trace
    float spdDist = spdsCopy.size() * this->sp->getDS();
    
    // begin training loop over link that have actual speed
    // measurements from start to end
    currRoute->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = currRoute->getLinks()->nextEntry();
    while(nextLink != NULL && !nextLink->value->isFinalLink())
    {
        // exclude first and last link since we only want links with
        // actual speed measurements over they're entirety
        if(nextLink->key == 0 || nextLink->key == currRoute->getLinks()->getSize() - 2)
        {
            nextLink = currRoute->getLinks()->nextEntry();
            continue;
        }
        
        std::cout << nextLink->value->getNumber() << std::endl;
        
        // get necessary road data
        Road* road_i = this->city->getRoads()->getEntry(nextLink->value->getNumber());
        float roadDist_i = road_i->getSplineLength();
        float roadSpdIndices_i = (float) roadDist_i / spdDist * spds->size();
        
        // vector containing actual speed values for given link
        std::vector<float> roadSpds_i;
        
        // get road speeds from actual trace of speed values.
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
    
    // train routpe prediction
    this->rp->parseRoute(currRoute);
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

bool DriverPrediction::allLinksHaveWeigths(Route* route)
{
    bool allLinksAreWeighted = true;
    
    route->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = route->getLinks()->nextEntry();
    while(nextLink != NULL)
    {
        if(!nextLink->value->linkHasWeights())
        {
            return false;
        }
        nextLink = route->getLinks()->nextEntry();
    }
    delete(nextLink);
    
    return allLinksAreWeighted;
}
    
} /* namespace PredictivePowertrain */
