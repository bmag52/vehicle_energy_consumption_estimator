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

Intersection* City::getIntersection(int intersectionNum) {
	assert(getInstersectionMapSize() < intersectionNum || intersectionNum < 1); // || this->intersections[intersectionNum] == NULL
	return this->intersections->getEntry(intersectionNum);
}

Route* City::getPath(Intersection* start, Intersection* end, std::vector<float>* conditions, int fastest) {
	GenericMap<int, int> dist;
	GenericMap<int, int> prev;
	GenericMap<int, int> unvisitedNodes;

	Intersection* startInt = getIntersection(start->getIntersectionID());
	Intersection* endInt = getIntersection(end->getIntersectionID());
	GenericMap<int, Intersection*>* adjInts = start->getAdjacentIntersection();

	dist.addEntry(start->getIntersectionID(), 0);
	prev.addEntry(start->getIntersectionID(), 0);

	adjInts->initializeCounter();
	GenericEntry<int, Intersection*>* nextIntersection = adjInts->nextEntry();
	while(nextIntersection != NULL)
	{
		int intNum = nextIntersection->value->getIntersectionID();
		dist.addEntry(intNum, INT_MAX);
		prev.addEntry(intNum, -1);
		nextIntersection = adjInts->nextEntry();
	}

	int closestIntNum = start->getIntersectionID();
	int distance = 0;
	while(distance != INT_MAX)
	{
		unvisitedNodes.addEntry(closestIntNum, INT_MAX);
		GenericMap<int, Intersection*>* neighbors = getIntersection(closestIntNum)->getAdjacentIntersection();
		neighbors->initializeCounter();
		GenericEntry<int, Intersection*>* nextNeighbor = neighbors->nextEntry();
		while(nextNeighbor != NULL)
		{
			int neighborNum = nextNeighbor->value->getIntersectionID();
            std::vector<float>* speedData = getConnectingRoad(getIntersection(closestIntNum), getIntersection(neighborNum))->getSpeedData();
			int spdAvg;
			size_t spdCount = speedData->size();
			for(size_t j = 0; j < spdCount; j++) { spdAvg += speedData->at(j); } spdAvg /= spdCount;
			int alt = dist.getEntry(closestIntNum) + spdCount / (fastest*spdAvg);
			if(dist.getEntry(neighborNum) > alt || unvisitedNodes.getEntry(neighborNum) == 0)
			{
				dist.addEntry(neighborNum, alt);
				prev.addEntry(neighborNum, closestIntNum);
				unvisitedNodes.addEntry(neighborNum, alt);
			}
			nextNeighbor = neighbors->nextEntry();
		}

		GenericEntry<int, int>* closestIntPair = unvisitedNodes.getMinEntry();
		distance = closestIntPair->key;
		closestIntNum = closestIntPair->value;
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

std::pair<std::vector<float>*, std::vector<float>*>* City::getRoadData(Link* link) {

    std::pair<std::vector<float>*, std::vector<float>*>* roadData = new std::pair<std::vector<float>*, std::vector<float>*>();

	if(link->getNumber() > getRoadMapSize() || link->isFinalLink())
	{
		return roadData;
	}

	roadData->first = this->roads->getEntry(link->getNumber())->getSpeedData();
	roadData->second = this->roads->getEntry(link->getNumber())->getElevData();

	if(link->getDirection())
	{
		roadData->first = reverseTrace(roadData->first);
		roadData->second = reverseTrace(roadData->second);
	}
	return roadData;
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
	GenericMap<int, Intersection*> passedInts;
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
					GenericEntry<int, Intersection*>* nextPassedInt = passedInts.nextEntry();
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

std::pair<std::vector<float>*, std::vector<float>*>* City::routeToData(Route* route, int dist) {

	int lastElev;
    std::vector<float>* speedData;
    std::vector<float>* elevData;

    std::pair<std::vector<float>*, std::vector<float>*>* data = new std::pair<std::vector<float>*, std::vector<float>*>();
    
	dist = dist / this->intervalDistance;
	bool firstLink = true;

	route->getLinks()->initializeCounter();
	GenericEntry<int, Link*>* nextLink = route->getLinks()->nextEntry();
	while(nextLink->value->isFinalLink())
	{
        std::pair<std::vector<float>*, std::vector<float>*>* roadData = getRoadData(nextLink->value);
		speedData = roadData->first;
		elevData = roadData->second;

		if(firstLink)
		{
			assert(dist <= speedData->size());

			dist = (int)std::max(dist, 1);
            
            std::vector<float>::iterator itr_elev = elevData->begin();
            std::vector<float>::iterator itr_spd = speedData->begin();
            while(itr_elev - elevData->begin() < dist && itr_spd - speedData->begin() < dist)
            {
                itr_elev = elevData->erase(itr_elev);
                itr_spd = speedData->erase(itr_spd);
            }

            std::pair<std::vector<float>*, float>* slopeData = elevationToSlope(elevData, 2*elevData->at(0) - elevData->at(1));
            data->first = speedData;
            data->second = slopeData->first;
            lastElev = slopeData->second;
            delete(slopeData);
            
			firstLink = false;
		} else {
            std::pair<std::vector<float>*, float>* slopeData = elevationToSlope(elevData, lastElev);
            std::vector<float> *slope = slopeData->first;
			lastElev = slopeData->second;

			// concatenate speed
            size_t allSpeedDataSize = data->first->size();
            size_t currSpeedDataSize = speedData->size();
            std::vector<float>* newSpeed = new std::vector<float>(allSpeedDataSize + currSpeedDataSize);
            for(int i = 0; i < allSpeedDataSize; i++) { newSpeed->push_back(data->first->at(i)); }
            for(int i = 0; i < currSpeedDataSize; i++) { newSpeed->push_back(speedData->at(i)); }
            delete(data->first);
            data->first = newSpeed;

			// concatenate slopes
            size_t allSlopeDataSize = data->second->size();
            size_t currSlopeDataSize = slopeData->first->size();
            std::vector<float>* newSlope = new std::vector<float>(allSlopeDataSize + currSlopeDataSize);
            for(int i = 0; i < allSlopeDataSize; i++) { newSlope->push_back(data->second->at(i)); }
            for(int i = 0; i < currSlopeDataSize; i++) { newSlope->push_back(slopeData->first->at(i)); }
            delete(data->second);
            data->second = newSlope;
            
            delete(slopeData);
		}
		nextLink = route->getLinks()->nextEntry();
	}
	return data;
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
