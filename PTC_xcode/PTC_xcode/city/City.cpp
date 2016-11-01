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

GenericMap<long int, Link*>* City::getNextLinks(Link* otherLink) {
	assert(!otherLink->isFinalLink());
	Road* currentRoad = this->roads->getEntry(otherLink->getNumber());
	Intersection* nextIntersection = getIntersectionFromLink(otherLink, true);

	GenericMap<long int, Link*>* nextLinks = new GenericMap<long int, Link*>();
	GenericMap<long int, Road*>* connectingRoads = nextIntersection->getRoads();

	long int count = 0;
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
	GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
	long int currentIntNum = end->getIntersectionID();
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

std::pair<std::vector<float>, std::vector<float>> City::getData(Road* road, int direction, SpeedPrediction* sp, Eigen::MatrixXd* spdIn, float distAlongRoad)
{
    // get elevation Data
    std::vector<float> elevData;
    std::vector<float> elevDistData;
    road->getElevData(&elevData, &elevDistData);
    
    if(direction)
    {
        for(int i = 0; i < elevData.size(); i++)
        {
            elevData.at(i) = elevData.at(elevData.size() - i - 1);
        }
    }
    assert(elevDistData.size() > 2);
    int prevElevMeasIdx = 0;

    // set output values
    std::vector<float> spdOut;
    std::vector<float> elevDataInterp;
    
    // iterate along speed trace to interpolate elevation trace
    float spdDist = 0.0;
    bool spdDistExceedsRoadDist = false;
    
    Eigen::MatrixXd spdOut_i(1, sp->getO());
    Eigen::MatrixXd spdIn_i(*spdIn);
    
    // format speed input
    sp->formatInData(&spdIn_i);
    
    while(!spdDistExceedsRoadDist)
    {
        // predict speed
        sp->predict(&spdIn_i, &spdOut_i);
        
        for(int i = 1; i < spdOut_i.cols(); i++)
        {
            spdDist += sp->getDS();
            
            if(spdDist > distAlongRoad && spdDist < road->getSplineLength())
            {
                // interpolate elevation data
                for(int j = prevElevMeasIdx; j < elevDistData.size() - 1; j++)
                {
                    if(spdDist > elevDistData.at(j))
                    {
                        prevElevMeasIdx = j;
                    }
                    else
                    {
                        break;
                    }
                }
                
                // get delta in elevation from previous to next elevation measurement
                float prev2NextElevDelta = elevData.at(prevElevMeasIdx) - elevData.at(prevElevMeasIdx + 1);
                
                // get delta in distnce from previous to next elevation measurement
                float next2PrevElevDist = elevDistData.at(prevElevMeasIdx + 1) - elevDistData.at(prevElevMeasIdx);
                
                // get delta in distance from previous elevation measurement to current speed distance
                float prevElev2SpdDist = spdDist - elevDistData.at(prevElevMeasIdx);
                
                // derive interpolation factor as a function prev 2 next elevation meas and speed distance after prev elev meas
                float interpFactor = prevElev2SpdDist / next2PrevElevDist;
                
                spdOut.push_back(spdOut_i.coeffRef(0, i) * sp->getMaxSpeed() - sp->getSpeedOffset());
                elevDataInterp.push_back(elevData.at(prevElevMeasIdx) + interpFactor * prev2NextElevDelta);
            }
            else if(spdDist > road->getSplineLength())
            {
                spdDistExceedsRoadDist = true;
                break;
            }
        }
        
        if(!spdDistExceedsRoadDist)
        {
            // place output into input and repeat if needed
            sp->output2Input(&spdIn_i, &spdOut_i);
            
            // perform quick train to prevent prediction discontinuities
            Eigen::MatrixXd spdAct_i(spdOut_i);
            for(int i = 0; i < 5; i++)
            {
                sp->predict(&spdIn_i, &spdOut_i);
                sp->train(&spdOut_i, &spdAct_i, &spdIn_i);
            }
        }
    }
    
    return std::pair<std::vector<float>, std::vector<float>>(spdOut, elevDataInterp);
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
    
	GenericMap<long int, Link*>* startLinks = startInt->getOutgoingLinks();
    GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
    
    // copy outgoing links to prevent mutilation
    startLinks->initializeCounter();
    GenericEntry<long int, Link*>* nextLinkEntry = startLinks->nextEntry();
    while(nextLinkEntry != NULL)
    {
        Link* outLink = nextLinkEntry->value;
        links->addEntry(nextLinkEntry->key, new Link(outLink->getDirection(), outLink->getNumber()));
        nextLinkEntry = startLinks->nextEntry();
    }
    delete(nextLinkEntry);
    
	GenericMap<long int, Link*>* path = new GenericMap<long int, Link*>();
	GenericMap<long int, Intersection*> passedInts;
	passedInts.addEntry(startInt->getIntersectionID(), startInt);
    
	for(int i = 0; i < totalLength; i++)
	{
		Link* nextLink = NULL;
		if(initialRoute->getLinkSize() > i)
		{
			links->initializeCounter();
			GenericEntry<long int, Link*>* nextLinkEntry = links->nextEntry();
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
			GenericEntry<long int, Link*>* nextLinkEntry = links->nextEntry();
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
		GenericMap<long int, Link*>* legalLinks = getNextLinks(routeCopy->getLinks()->getEntry(1));
		legalLinks->initializeCounter();
		GenericEntry<long int, Link*>* nextLink = legalLinks->nextEntry();
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

std::pair<std::vector<float>, std::vector<float>> City::routeToData(Route* route, float dist, SpeedPrediction* sp, Eigen::MatrixXd* spdIn) {

    std::vector<float> elevData;
    std::vector<float> spdData;
    
    Eigen::MatrixXd spdIn_i(*spdIn);

    bool isFirstLink = true;
	route->getLinks()->initializeCounter();
	GenericEntry<long int, Link*>* nextLink = route->getLinks()->nextEntry();
	while(nextLink != NULL)
	{
        if(nextLink->value->isFinalLink())
        {
            delete(nextLink);
            return std::pair<std::vector<float>, std::vector<float>>(spdData, elevData);
        }
        
        // get road from link
        assert(this->roads->hasEntry(nextLink->value->getNumber()));
        Road* roadFromLink = this->roads->getEntry(nextLink->value->getNumber());
    
        float distAlongLink = 0.0;
        if(isFirstLink)
        {
            distAlongLink = dist;
            isFirstLink = false;
        }
        
        // get spd and elevation data
        sp->setVals(nextLink->value->getWeights(nextLink->value->getDirection()));
        std::pair<std::vector<float>, std::vector<float>> linkData = this->getData(roadFromLink, nextLink->value->getDirection(), sp, &spdIn_i, distAlongLink);
        std::vector<float> spdOut_i = linkData.first;
        std::vector<float> elevData_i = linkData.second;

        // concatenate data
        for(int i = 0; i < spdOut_i.size(); i++) { spdData.push_back(spdOut_i.at(i)); }
        for(int i = 0; i < elevData_i.size(); i++) { elevData.push_back(elevData_i.at(i)); }
        
		nextLink = route->getLinks()->nextEntry();
	}
    delete(nextLink);
    return std::pair<std::vector<float>, std::vector<float>>(spdData, elevData);
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
