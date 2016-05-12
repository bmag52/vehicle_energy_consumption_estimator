/*
 * CityObj.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "City.h"

namespace PredictivePowertrain {

City::City() {
	// TODO Auto-generated constructor stub
}

City::City(GenericMap<int, Intersection*>* intersections, GenericMap<long int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) {
	this->roads = roads;
	this->intersections = intersections;
	this->boundsMap = boundsMap;
}

int City::getRoadMapSize() {
	return this->roads->getSize();
}

int City::getInstersectionMapSize() {
	return this->intersections->getSize();
}

GenericMap<int, Link*>* City::getNextLinks(Link* link) {
	assert(link->isEqual(this->link->finalLink()));
	Road* currentRoad = this->roads->getEntry(link->getNumber());
	Intersection* nextIntersection = getIntersectionFromLink(link, true);

	GenericMap<int, Link*>* nexLinks = new GenericMap<int, Link*>();
	GenericMap<long int, Road*>* connectingRoads = nextIntersection->getRoads();

	int count = 0;
	connectingRoads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
	while(nextRoad != NULL)
	{
		if(nextRoad->value->getRoadID() != currentRoad->getRoadID())
		{
			Link* newLink = link->linkFromRoad(nextRoad->value, nextIntersection);
			nexLinks->addEntry(count++, newLink);
		}
		nextRoad = connectingRoads->nextEntry();
	}
	nexLinks->addEntry(count, this->link->finalLink());
	return nexLinks;
}

Intersection* City::getIntersectionFromLink(Link* link, bool isIntersection) {
	assert(!link->isEqual(this->link->finalLink()));
	Road* road = this->roads->getEntry(link->getNumber());

	if(((link->getDirection() == 0) + isIntersection) % 2)
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

Route* City::getPath(Intersection* start, Intersection* end, int* conditions, int fastest) {
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
			int* speedData = getConnectingRoad(getIntersection(closestIntNum), getIntersection(neighborNum))->getSpeedData();
			int spdAvg;
			int spdCount = sizeof(*speedData)/sizeof(int);
			for(int j = 0; j < spdCount; j++) { spdAvg+=speedData[j]; } spdAvg /= spdCount;
			int alt = dist.getEntry(closestIntNum) + spdCount/(fastest*spdAvg);
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

Intersection* City::addIntersection(Intersection* intersections, Intersection* intersection) {
	int intCount = sizeof(*intersection)/sizeof(Intersection);
	Intersection* newInts = new Intersection[intCount+1];
	for(int i = 0; i < intCount; i++)
	{
		newInts[i] = intersections[i];
	}
	newInts[intCount + 1] = *intersection;
	free(intersections);
	return newInts;
}

int* City::reverseTrace(int* trace)
{
	int traceSize = sizeof(*trace)/sizeof(int);
	int* newTrace = new int[traceSize];
	for(int i = 0; i < traceSize; i++)
	{
		newTrace[i] = trace[traceSize - i];
	}
	free(trace);
	return newTrace;
}

std::pair<int*, int*>* City::getRoadData(Link* link) {

	std::pair<int*, int*>* roadData;

	if(link->getNumber() > getRoadMapSize() || link->isEqual(this->link->finalLink()))
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

Route* City::randomPath(Intersection* startInt, Route* initialRoute, int totalLength, int* conditions) {
	GenericMap<int, Link*>* links = startInt->getOutgoingLinks();
	GenericMap<int, Link*>* path;
	GenericMap<int, Intersection*>* passedInts;

	passedInts->addEntry(startInt->getIntersectionID(), startInt);
	for(int i = 0; i < totalLength; i++)
	{
		Link* nextLink = NULL;
		if(initialRoute->getLinkSize() >= i)
		{
			links->initializeCounter();
			GenericEntry<int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(nextLinkEntry->value->isEqual(initialRoute->getLinksPtr()->getEntry(i)))
				{
					nextLink = initialRoute->getLinksPtr()->getEntry(i);
					break;
				}
				nextLinkEntry = links->nextEntry();
			}
		}
		if(nextLink != NULL)
		{
			links->initializeCounter();
			GenericEntry<int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(!nextLinkEntry->value->isEqual(this->link->finalLink()))
				{
					Intersection* intersection = getIntersectionFromLink(nextLinkEntry->value, true);

					int linkCount = 1;
					passedInts->initializeCounter();
					GenericEntry<int, Intersection*>* nextPassedInt = passedInts->nextEntry();
					while(nextPassedInt != NULL)
					{
						if(intersection->getIntersectionID() == nextPassedInt->value->getIntersectionID())
						{
							links->erase(linkCount);
						}
						linkCount++;
						nextPassedInt = passedInts->nextEntry();
					}
				}
				nextLinkEntry = links->nextEntry();
				if(links->getSize() == 1) { break; }
				int randIdx = static_cast<int>(rand()*(links->getSize()-1));
				nextLink = links->getEntry(randIdx);
			}
		}
		while(nextLink->isEqual(this->link->finalLink()) && links->getSize() > 1)
		{
			int randIdx = static_cast<int>(rand()*links->getSize());
			nextLink = links->getEntry(randIdx);
		}
		Intersection* newPassedInt = getIntersectionFromLink(nextLink, true);
		passedInts->addEntry(newPassedInt->getIntersectionID(), newPassedInt);
		path->addEntry(i+1, nextLink);
		links = getNextLinks(nextLink);
	}
	path->addEntry(totalLength+1, this->link->finalLink());
	Goal* goal = new Goal(getIntersectionFromLink(path->getEntry(path->getSize()-2),true)->getIntersectionID(), conditions);
	Route *route = new Route(path, goal);
	return route;
}

bool City::legalRoute(Route* route) {
	Route* routeCopy = route->copy();
	while(routeCopy->getLinkSize() > 1)
	{
		GenericMap<int, Link*>* legalLinks = getNextLinks(routeCopy->getLinksPtr()->getEntry(1));
		legalLinks->initializeCounter();
		GenericEntry<int, Link*>* nextLink = legalLinks->nextEntry();
		bool error = true;
		while(nextLink != NULL)
		{
			if(nextLink->value->isEqual(routeCopy->getLinksPtr()->getEntry(1)))
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

std::pair<double*, int>* City::elevationToSlope(int* elev, int oldElev) {
	int elevSize = sizeof(*elev)/sizeof(int);
	double* slope = new double[elevSize];

	slope[0] = (elev[0] - oldElev) / this->intervalDistance;
	for(int i = 1; i < elevSize; i++) { slope[i] = (elev[i] - elev[i-1]) / this->intervalDistance; }
	for(int i = 0; i < elevSize; i++)
	{
		if(this->maxSlopePercent/100 < slope[i])
		{
			slope[i] = this->maxSlopePercent/100;
		}
		slope[i] = atan(slope[i]);
	}
	int endElev = elev[elevSize-1];

	std::pair<double*, int>* data;
	data->first = slope;
	data->second = endElev;
	return data;
}

std::pair<int*, double*>* City::routeToData(Route* route, int dist) {
	int* speedData;
	double* slopeData;

	int lastElev;
	int* speed;
	int* elevData;

	dist = dist / this->intervalDistance;
	bool firstLink = true;

	route->getLinksPtr()->initializeCounter();
	GenericEntry<int, Link*>* nextLink = route->getLinksPtr()->nextEntry();
	while(nextLink->value->isEqual(this->link->finalLink()))
	{
		std::pair<int*, int*>* roadData = getRoadData(nextLink->value);
		speed = roadData->first;
		elevData = roadData->second;

		if(firstLink)
		{
			assert(dist <= sizeof(*speed)/sizeof(int));

			dist = (int)std::max(dist, 1);
			speedData = &speed[dist];
			elevData  = &elevData[dist];
			std::pair<double*, int>* data = elevationToSlope(elevData, 2*elevData[0]-elevData[1]);
			slopeData = data->first;
			lastElev = data->second;

			firstLink = false;
		} else {
			std::pair<double*, int>* data = elevationToSlope(elevData, lastElev);
			double* slope = data->first;
			lastElev = data->second;

			// concatenate speed
			int speedDataSize = sizeof(*speedData)/sizeof(int);
			int speedSize = sizeof(*speed)/sizeof(int);
			int* newSpeedData = new int[speedDataSize + speedSize];
			for(int i = 0; i < speedDataSize; i++) { newSpeedData[i] = speedData[i]; }
			for(int i = speedDataSize - 1; i < speedDataSize + speedSize; i++) { newSpeedData[i] = speed[i - speedSize - 1]; }
			speedData = newSpeedData;

			// concatenate slopes
			int slopeDataSize = sizeof(*slopeData)/sizeof(double);
			int slopeSize = sizeof(*slope)/sizeof(double);
			double* newSlopeData = new double[slopeDataSize + slopeSize];
			for(int i = 0; i < slopeDataSize; i++) { newSlopeData[i] = slopeData[i]; }
			for(int i = slopeDataSize - 1; i < slopeDataSize + slopeSize; i++) { newSlopeData[i] = slope[i - slopeSize - 1]; }
			slopeData = newSlopeData;
		}
		nextLink = route->getLinksPtr()->nextEntry();
	}
	std::pair<int*, double*>* data;
	data->first = speedData;
	data->second = slopeData;
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

GenericMap<int, Intersection*>* City::getIntersections() {
	return this->intersections;
}

} /* namespace PredictivePowertrain */
