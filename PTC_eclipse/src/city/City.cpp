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

City::City(Intersection* intersections, Road* roads, GenericMap<int, Bounds*>* boundsMap) {
	this->roads = roads;
	this->intersections = intersections;
	this->boundsMap = boundsMap;
}

int City::getRoadListSize() {
	return sizeof(*this->roads)/sizeof(Road);
}

int City::getInstersectionListSize() {
	return sizeof(*this->roads)/sizeof(Road);
}

Link* City::getNextLinks(Link* link) {
	assert(link->isEqual(this->link->finalLink()));
	Road* currentRoad = &this->roads[link->getNumber()];
	Intersection* nextIntersection = getIntersectionFromLink(link,true);

	Link* links;
	int linkSize = 0;
	for(int i = 0; i < nextIntersection->getRoadCount(); i++)
	{
		if(nextIntersection->getRoads()[i].getRoadID() != currentRoad->getRoadID())
		{
			linkSize++;
			Link* newLinks = new Link[linkSize+1];
			newLinks[0] = *link->linkFromRoad(&nextIntersection->getRoads()[i], nextIntersection);
			for(int j = 1; j < linkSize; j++) { newLinks[i] = links[i-1]; }
			free(links);
			links = newLinks;
		}
	}
	links[linkSize+1] = *this->link->finalLink();
	return links;
}

Intersection* City::getIntersectionFromLink(Link* link, bool isIntersection) {
	assert(!link->isEqual(this->link->finalLink()));
	Road* road = &this->roads[link->getNumber()];

	if(((link->getDirection() == 0) + isIntersection) % 2)
	{
		return road->getStartNode();
	} else {
		return road->getEndNode();
	}
}

Intersection* City::getIntersection(int intersectionNum) {
	assert(getInstersectionListSize() < intersectionNum || intersectionNum < 1); // || this->intersections[intersectionNum] == NULL
	return &this->intersections[intersectionNum];
}

Route* City::getPath(Intersection* start, Intersection* end, int* conditions, int fastest) {
	GenericMap<int, int> dist;
	GenericMap<int, int> prev;
	GenericMap<int, int> unvisitedNodes;

	Intersection* startInt = getIntersection(start->getNumber());
	Intersection* endInt = getIntersection(end->getNumber());
	Intersection* adjInts = start->getAdjacentIntersection();

	dist.addEntry(start->getNumber(), 0);
	prev.addEntry(start->getNumber(), 0);
	for(int i = 0; i < sizeof(*adjInts)/sizeof(Intersection); i++)
	{
		int intNum = adjInts[i].getNumber();
		dist.addEntry(intNum, INT_MAX);
		prev.addEntry(intNum, -1);
	}

	int closestIntNum = start->getNumber();
	int distance = 0;
	while(distance != INT_MAX)
	{
		unvisitedNodes.addEntry(closestIntNum, INT_MAX);
		Intersection* neighbors = getIntersection(closestIntNum)->getAdjacentIntersection();
		for(int i = 0; i < sizeof(*neighbors)/sizeof(Intersection); i++)
		{
			int neighborNum = neighbors[i].getNumber();
			int* speedData = getConnectingRoad(getIntersection(closestIntNum), getIntersection(neighborNum))->getSpeedData();
			int spdAvg;
			int spdCount = sizeof(*speedData)/sizeof(int);
			for(int j = 0; j < spdCount; j++) { spdAvg+=speedData[i]; } spdAvg /= spdCount;
			int alt = dist.getEntry(closestIntNum) + spdCount/(fastest*spdAvg);
			if(dist.getEntry(neighborNum) > alt || unvisitedNodes.getEntry(neighborNum) == 0)
			{
				dist.addEntry(neighborNum, alt);
				prev.addEntry(neighborNum, closestIntNum);
				unvisitedNodes.addEntry(neighborNum, alt);
			}
		}

		GenericEntry<int, int>* closestIntPair = unvisitedNodes.getMinEntry();
		distance = closestIntPair->key;
		closestIntNum = closestIntPair->value;
	}

	Link* links;
	int currentIntNum = end->getNumber();
	while(currentIntNum != start->getNumber())
	{
		Intersection* previousInt = getIntersection(prev.getEntry(currentIntNum));
		links = addLink(links, this->link->linkFromRoad(getConnectingRoad(previousInt, getIntersection(currentIntNum)), previousInt));
		currentIntNum = previousInt->getNumber();
	}

	int linkCount = sizeof(*links)/sizeof(Link);
	Link* newLinks = new Link[linkCount+1];
	for(int i = 0; i < linkCount; i++)
	{
		newLinks[i] = links[linkCount-i];
	}
	newLinks[linkCount+1] = *this->link->finalLink();
	free(links);
	Goal goal(end->getNumber(), conditions);
	Route route(newLinks, &goal);
	return &route;
}

Link* City::addLink(Link* links, Link* link) {
	int linkCount = sizeof(*links)/sizeof(int);
	Link* newLinks = new Link[linkCount+1];
	for(int i = 0; i < linkCount; i++)
	{
		newLinks[i] = links[i];
	}
	newLinks[linkCount+1] = *link;
	free(links);
	return newLinks;
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

City::~City() {
	// TODO Auto-generated destructor stub
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

	if(link->getNumber() > getRoadListSize() || link->isEqual(this->link->finalLink()))
	{
		return roadData;
	}

	roadData->first = this->roads[link->getNumber()].getSpeedData();
	roadData->second = this->roads[link->getNumber()].getElevData();

	if(link->getDirection())
	{
		roadData->first = reverseTrace(roadData->first);
		roadData->second = reverseTrace(roadData->second);
	}
	return roadData;
}

Road* City::getConnectingRoad(Intersection* one, Intersection* two) {
	Road* roads = one->getRoads();
	for(int i = 0; i < sizeof(*roads)/sizeof(Road); i++)
	{
		if(one->getNextIntersection(&roads[i])->getNumber() == two->getNumber())
		{
			return &roads[i];
		}
	}
}

Route* City::randomPath(Intersection* startInt, Route* initialRoute, int totalLength, int* conditions) {
	Link* links = startInt->getOutgoingLinks();
	Link* path = {};
	int linkCount = sizeof(*link)/sizeof(Link);
	Intersection* passedInts = {startInt};
	for(int i = 0; i < totalLength; i++)
	{
		Link* nextLink = NULL;
		if(initialRoute->getLinkSize() >= i)
		{
			for(int j = 0; j < linkCount; j++)
			{
				if(links[j].isEqual(&initialRoute->getLinksPtr()[i]))
				{
					nextLink = &initialRoute->getLinksPtr()[i];
					break;
				}
			}
		}
		if(nextLink != NULL)
		{
			for(int j = sizeof(*links)/sizeof(Link)-1; j >= 0; j--)
			{
				if(!links[j].isEqual(this->link->finalLink()))
				{
					Intersection* intersection = getIntersectionFromLink(&links[j], true);
					for(int k = 0; k < sizeof(*passedInts)/sizeof(Intersection); k++)
					{
						if(intersection->getNumber() == passedInts[k].getNumber())
						{
							linkCount--;
							Link* newLinks = new Link[linkCount];
							int itr = 0;
							for(int l = 0; l < linkCount; l++)
							{
								if(itr == k) { itr++; }
								newLinks[l] = links[itr];
								itr++;
							}
							free(links);
							links = newLinks;
							break;
						}
					}
				}
				if(linkCount == 1) { break; }
				int randIdx = static_cast<int>(rand()*(linkCount-1));
				nextLink = &links[randIdx];
			}
		}
		while(nextLink->isEqual(this->link->finalLink()) && linkCount > 1)
		{
			int randIdx = static_cast<int>(rand()*linkCount);
			nextLink = &links[randIdx];
		}
		passedInts = addIntersection(passedInts, getIntersectionFromLink(nextLink, true));
		path = addLink(path, nextLink);
		links = getNextLinks(nextLink);
	}
	path = addLink(path, this->link->finalLink());
	int pathSize = sizeof(*path)/sizeof(Link);
	Goal goal(getIntersectionFromLink(&path[pathSize-2],true)->getNumber(), conditions);
	Route route(path, &goal);
	return &route;
}

bool City::legalRoute(Route* route) {
	Route* routeCopy = route->copy();
	while(routeCopy->getLinkSize() > 1)
	{
		Link* legalLinks = getNextLinks(&(routeCopy->getLinksPtr()[0]));
		bool error = true;
		for(int i = 0; i < routeCopy->getLinkSize(); i++)
		{
			if(legalLinks[i].isEqual(&(routeCopy->getLinksPtr()[1])))
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

	Link* link = route->nextlink();
	dist = dist / this->intervalDistance;
	bool firstLink = true;

	while(link->isEqual(this->link->finalLink()))
	{
		std::pair<int*, int*>* roadData = getRoadData(link);
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
		link = route->nextlink();
	}
	std::pair<int*, double*>* data;
	data->first = speedData;
	data->second = slopeData;
	return data;
}

GenericMap<int, Bounds*>* City::getBoundsMap() {
	return this->boundsMap;
}

Road* City::getRoads() {
	return this->roads;
}

Intersection* City::getIntersection() {
	return this->intersections;
}

} /* namespace PredictivePowertrain */
