/*
 * CityObj.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "City.h"

namespace PredictivePowertrain {

City::City() {
    this->roads = new GenericMap<long int, Road*>();
    this->intersections = new GenericMap<long int, Intersection*>();
    this->boundsMap = new GenericMap<int, Bounds*>();
    std::srand(std::time(0));
}

City::City(GenericMap<long int, Intersection*>* intersections, GenericMap<long int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) {
	this->roads = roads;
	this->intersections = intersections;
	this->boundsMap = boundsMap;
    std::srand(std::time(0));
}
    
City::~City()
{
    delete(this->roads);
    delete(this->intersections);
    delete(this->boundsMap);
}

int City::getRoadMapSize() {
	return this->roads->getSize();
}

int City::getInstersectionMapSize() {
	return this->intersections->getSize();
}

GenericMap<int, Link*>* City::getNextLinks(Link* otherLink) {
	assert(!otherLink->isFinalLink());
	Road* currentRoad = this->roads->getEntry(otherLink->getNumber());
	Intersection* nextIntersection = getIntersectionFromLink(otherLink, true);

	GenericMap<int, Link*>* nextLinks = new GenericMap<int, Link*>();
	GenericMap<long int, Road*>* connectingRoads = nextIntersection->getRoads();

	int count = 0;
	connectingRoads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
	while(nextRoad != NULL)
	{
		if(nextRoad->value->getRoadID() != currentRoad->getRoadID())
		{
			Link* newLink = otherLink->linkFromRoad(nextRoad->value, nextIntersection);
			nextLinks->addEntry(count, newLink);
            count++;
		}
		nextRoad = connectingRoads->nextEntry();
	}
    delete(nextRoad);
	nextLinks->addEntry(count, this->link->finalLink());
	return nextLinks;
}

Intersection* City::getIntersectionFromLink(Link* link, bool isIntersection) {
	assert(!link->isFinalLink());
	Road* road = this->roads->getEntry(link->getNumber());

	if(!(link->getDirection() == 0) != !isIntersection)
	{
		return road->getStartIntersection();
	} else {
		return road->getEndIntersection();
	}
}

Intersection* City::getIntersection(long int intersectionNum) {
	assert(getInstersectionMapSize() < intersectionNum || intersectionNum < 1); // || this->intersections[intersectionNum] == NULL
	return this->intersections->getEntry(intersectionNum);
}

Route* City::getPath(Intersection* start, Intersection* end, std::vector<float>* conditions, int fastest) {
	GenericMap<long int, float> dist;
	GenericMap<long int, long int> prev;
	GenericMap<long int, float> unvisitedNodes;

	GenericMap<long int, Intersection*>* adjInts = start->getAdjacentIntersections();

	adjInts->initializeCounter();
	GenericEntry<long int, Intersection*>* nextIntersection = adjInts->nextEntry();
	while(nextIntersection != NULL)
	{
		long int intNum = nextIntersection->value->getIntersectionID();
		dist.addEntry(intNum, FLT_MAX);
		prev.addEntry(intNum, -1);
		nextIntersection = adjInts->nextEntry();
	}
    delete(nextIntersection);
    
    dist.addEntry(start->getIntersectionID(), 0);

	long int closestIntNum = start->getIntersectionID();
	long int distance = 0;
	while(distance != FLT_MAX)
	{
		unvisitedNodes.addEntry(closestIntNum, FLT_MAX);
		GenericMap<long int, Intersection*>* neighbors = getIntersection(closestIntNum)->getAdjacentIntersections();
        
		neighbors->initializeCounter();
		GenericEntry<long int, Intersection*>* nextNeighbor = neighbors->nextEntry();
		while(nextNeighbor != NULL)
		{
			long int neighborNum = nextNeighbor->value->getIntersectionID();
            
            Road* connectingRoad = getConnectingRoad(getIntersection(closestIntNum), nextNeighbor->value);
            
			float alt = dist.getEntry(neighborNum) + connectingRoad->getSplineLength();
			if(dist.getEntry(neighborNum) > alt || !unvisitedNodes.hasEntry(neighborNum))
			{
				dist.addEntry(neighborNum, alt);
				prev.addEntry(neighborNum, closestIntNum);
				unvisitedNodes.addEntry(neighborNum, alt);
			}
			nextNeighbor = neighbors->nextEntry();
		}

		GenericEntry<long int, float>* closestIntPair = unvisitedNodes.getMinEntry();
		distance = closestIntPair->value;
		closestIntNum = closestIntPair->key;
	}

	int linkCount = 0;
	GenericMap<int, Link*>* links = new GenericMap<int, Link*>();
	int currentIntNum = end->getIntersectionID();
	while(currentIntNum != start->getIntersectionID())
	{
		Intersection* previousInt = getIntersection(prev.getEntry(currentIntNum));
		links->addEntry(linkCount++, this->link->linkFromRoad(getConnectingRoad(previousInt, getIntersection(currentIntNum)), previousInt));
		currentIntNum = previousInt->getIntersectionID();
	}

	links->addEntry(linkCount++, this->link->finalLink());
	Goal* goal = new Goal(end->getIntersectionID(), conditions);
	Route* route = new Route(links, goal);
	return route;
}

std::vector<float>* City::reverseTrace(std::vector<float>* trace)
{
    std::vector<float>* newTrace = new std::vector<float>(trace->size());
	for(int i = 0; i < trace->size(); i++)
	{
		newTrace->at(i) = trace->at(trace->size() - i);
	}
	delete(trace);
	return newTrace;
}

std::pair<std::vector<float>*, std::vector<float>*> City::getData(Link* link, SpeedPrediction* sp, Eigen::MatrixXd* spdIn)
{
    assert(!link->isFinalLink() && this->roads->hasEntry(link->getNumber()));
    
    Road* roadFromLink = this->roads->getEntry(link->getNumber());
    
    // get speed Data
    std::vector<float>* spdOut = new std::vector<float>();
    
    float spdDist = 0.0;
    sp->setVals(link->getWeights(link->getDirection()));
    
    Eigen::MatrixXd spdOut_i(1, sp->getO());
    Eigen::MatrixXd spdIn_i(*spdIn);
    
    while(spdDist < roadFromLink->getSplineLength())
    {
        // predict speed
        sp->predict(&spdIn_i, &spdOut_i);
        
        float spdDist = 0.0;
        float prevSpd = spdOut_i.coeffRef(0, 0);
        
        for(int i = 1; i < spdOut_i.size(); i++)
        {
            float currSpd = spdOut_i.coeffRef(0, i);
            float accel_i = (currSpd - prevSpd) / sp->getDT();
            float dist_i = prevSpd + 0.5 * accel_i * std::pow(sp->getDT(),2);
            
            spdOut->push_back(spdOut_i.coeffRef(0, i));
            
            spdDist += dist_i;
        }
        
        // assume out is always greater than in and update speed input from output
        spdIn_i = spdOut_i.block(0, spdOut_i.cols() - 1 - sp->getI(), 1, sp->getI());
    }
    
    // get elevation Data
    std::vector<float>* elevData = roadFromLink->getSpeedData();

	if(link->getDirection())
	{
		elevData = reverseTrace(elevData);
	}
    return std::pair<std::vector<float>*, std::vector<float>*>(spdOut, elevData);
}

Road* City::getConnectingRoad(Intersection* one, Intersection* two) {
	GenericMap<long int, Road*>* roads = one->getRoads();
	roads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = roads->nextEntry();
	while(nextRoad != NULL)
	{
		if(one->getNextIntersection(nextRoad->value)->getIntersectionID() == two->getIntersectionID())
		{
			return nextRoad->value;
		}
		nextRoad = roads->nextEntry();
	}
	return NULL;
}

Route* City::randomPath(Intersection* startInt, Route* initialRoute, int totalLength, std::vector<float>* conditions) {
    
	GenericMap<int, Link*>* startLinks = startInt->getOutgoingLinks();
    GenericMap<int, Link*>* links = new GenericMap<int, Link*>();
    
    // copy outgoing links to prevent mutilation
    startLinks->initializeCounter();
    GenericEntry<int, Link*>* nextLinkEntry = startLinks->nextEntry();
    while(nextLinkEntry != NULL)
    {
        Link* outLink = nextLinkEntry->value;
        links->addEntry(nextLinkEntry->key, new Link(outLink->getDirection(), outLink->getNumber()));
        nextLinkEntry = startLinks->nextEntry();
    }
    delete(nextLinkEntry);
    
	GenericMap<int, Link*>* path = new GenericMap<int, Link*>();
	GenericMap<long int, Intersection*> passedInts;
	passedInts.addEntry(startInt->getIntersectionID(), startInt);
    
	for(int i = 0; i < totalLength; i++)
	{
		Link* nextLink = NULL;
		if(initialRoute->getLinkSize() > i)
		{
			links->initializeCounter();
			GenericEntry<int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(nextLinkEntry->value->isEqual(initialRoute->getLinks()->getEntry(i)))
				{
					nextLink = initialRoute->getLinks()->getEntry(i);
					break;
				}
				nextLinkEntry = links->nextEntry();
			}
            delete(nextLinkEntry);
		}
        
		if(nextLink == NULL)
		{
			links->initializeCounter();
			GenericEntry<int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(!nextLinkEntry->value->isFinalLink())
				{
					Intersection* intersection = getIntersectionFromLink(nextLinkEntry->value, true);

					passedInts.initializeCounter();
					GenericEntry<long int, Intersection*>* nextPassedInt = passedInts.nextEntry();
					while(nextPassedInt != NULL)
					{
						if(intersection->getIntersectionID() == nextPassedInt->value->getIntersectionID())
						{
							links->indexErase(nextLinkEntry->key);
						}
						nextPassedInt = passedInts.nextEntry();
					}
                    delete(nextPassedInt);
				}
                nextLinkEntry = links->nextEntry();
            }
            delete(nextLinkEntry);
            
            // break of no next link
            if(links->getSize() == 1) { break; }
            
            int randIdx = std::floor((float)std::rand() / RAND_MAX * links->getSize());
            nextLink = links->getEntry(randIdx);
		}
        
		while(nextLink->isFinalLink() && links->getSize() > 1 && nextLink != NULL)
		{
            int randIdx = std::floor((float)std::rand() / RAND_MAX * links->getSize());
			nextLink = links->getEntry(randIdx);
		}
        
		Intersection* newPassedInt = getIntersectionFromLink(nextLink, true);
		passedInts.addEntry(newPassedInt->getIntersectionID(), newPassedInt);
		path->addEntry(i, nextLink);
        
        delete(links);
		links = getNextLinks(nextLink);
	}
	path->addEntry(path->getSize(), this->link->finalLink());
	Goal* goal = new Goal(getIntersectionFromLink(path->getEntry(path->getSize()-2),true)->getIntersectionID(), conditions);
	Route *route = new Route(path, goal);
    
	return route;
}

bool City::legalRoute(Route* route) {
	Route* routeCopy = route->copy();
	while(routeCopy->getLinkSize() > 1)
	{
		GenericMap<int, Link*>* legalLinks = getNextLinks(routeCopy->getLinks()->getEntry(1));
		legalLinks->initializeCounter();
		GenericEntry<int, Link*>* nextLink = legalLinks->nextEntry();
		bool error = true;
		while(nextLink != NULL)
		{
			if(nextLink->value->isEqual(routeCopy->getLinks()->getEntry(1)))
			{
				error = false;
				break;
			}
		}
		if(error)
		{
			return false;
		}
		routeCopy->removeFirstLink();
	}
	return true;
}

std::pair<std::vector<float>*, float>* City::elevationToSlope(std::vector<float>* elev, float oldElev)
{
	size_t elevSize = elev->size();
    std::vector<float>* slope = new std::vector<float>(elevSize);

	slope->at(0) = (elev->at(0) - oldElev) / this->intervalDistance;
	for(int i = 1; i < elevSize; i++) { slope->at(i) = (elev->at(i) - elev->at(i-1)) / this->intervalDistance; }
	for(int i = 0; i < elevSize; i++)
	{
		if(this->maxSlopePercent / 100 < slope->at(i))
		{
			slope->at(i) = this->maxSlopePercent / 100;
		}
        slope->at(i) = std::atan(slope->at(i));
	}
    float endElev = elev->at(elevSize - 1);

    std::pair<std::vector<float>*, float>* data;
	data->first = slope;
	data->second = endElev;
	return data;
}
    
bool City::trimSpeedTrace(Road* road, float distAlongRoad, float dt, std::vector<float>* spdOut)
{
    float roadDist = road->getSplineLength();
    
    float spdDist = 0.0;
    float prevSpd = spdOut->at(0);
    for(int i = 1; i < spdOut->size(); i++)
    {
        float currSpd = spdOut->at(i);
        float accel_i = (currSpd - prevSpd) / dt;
        float dist_i = prevSpd + 0.5 * accel_i * std::pow(dt,2);
        
        spdDist += dist_i;
        
        if(spdDist < distAlongRoad)
        {
            spdOut->erase(spdOut->begin());
        }
        
        else if(spdDist > roadDist)
        {
            for(int j = i; j < spdOut->size(); j++)
            {
                spdOut->pop_back();
            }
            return true;
        }
    }
    return false;
}

std::pair<std::vector<float>*, std::vector<float>*> City::routeToData(Route* route, int distIndex, SpeedPrediction* sp, Eigen::MatrixXd* spdIn) {

    std::vector<float>* elevData = new std::vector<float>();
    std::vector<float>* spdData = new std::vector<float>();
    
    Eigen::MatrixXd spdIn_i(*spdIn);

    bool isFirstLink = true;
	route->getLinks()->initializeCounter();
	GenericEntry<int, Link*>* nextLink = route->getLinks()->nextEntry();
	while(nextLink->value->isFinalLink())
	{
        // get data
        Road* roadFromLink = this->roads->getEntry(nextLink->value->getNumber());
        
        // get spd and elevation data
        std::pair<std::vector<float>*, std::vector<float>*> linkData = this->getData(nextLink->value, sp, &spdIn_i);
        std::vector<float>* spdOut_i = linkData.first;
        std::vector<float>* elevData_i = linkData.second;

        if(isFirstLink)
        {
            float distAlongRoad = (float) distIndex / roadFromLink->getNodes()->getSize() * roadFromLink->getSplineLength();
            this->trimSpeedTrace(roadFromLink, distAlongRoad, sp->getDT(), spdOut_i);
            
            std::vector<float>::iterator itrElev = elevData_i->begin();
            while(itrElev - elevData->begin() < distIndex)
            {
                itrElev = elevData->erase(itrElev);
            }
            
            isFirstLink = false;
        }
        else
        {
            this->trimSpeedTrace(roadFromLink, 0.0, sp->getDT(), spdOut_i);
        }

        // concatenate data
        for(int i = 0; i < spdOut_i->size(); i++) { spdData->push_back(spdOut_i->at(i)); }
        for(int i = 0; i < elevData_i->size(); i++) { elevData->push_back(elevData_i->at(i)); }
        delete(spdOut_i);
        delete(elevData_i);
        
		nextLink = route->getLinks()->nextEntry();
	}
    delete(nextLink);
    return std::pair<std::vector<float>*, std::vector<float>*>(spdData, elevData);
}

GenericMap<int, Bounds*>* City::getBoundsMap() {
	return this->boundsMap;
}

GenericMap<long int, Road*>* City::getRoads() {
	return this->roads;
}

int City::getBoundsMapSize() {
	return this->boundsMap->getSize();
}

GenericMap<long int, Intersection*>* City::getIntersections() {
	return this->intersections;
}

} /* namespace PredictivePowertrain */
