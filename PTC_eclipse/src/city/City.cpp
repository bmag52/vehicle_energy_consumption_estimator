/*
 * CityObj.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "City.h"

namespace PredictivePowertrain {

CityObj::CityObj() {
	// TODO Auto-generated constructor stub
}

int CityObj::getRoadListSize() {
	return sizeof(*this->roadList)/sizeof(Road);
}

int CityObj::getInstersectionListSize() {
	return sizeof(*this->roadList)/sizeof(Road);
}

Link* CityObj::getNextLinks(Link* link) {
	assert(link->isEqual(this->link->finalLink()));
	Road* currentRoad = &this->roadList[link->getNumber()];
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

Intersection* CityObj::getIntersectionFromLink(Link* link, bool isIntersection) {
	assert(!link->isEqual(this->link->finalLink()));
	Road* road = &this->roadList[link->getNumber()];

	if(((link->getDirection() == 0) + isIntersection) % 2)
	{
		return road->getStartNode();
	} else {
		return road->getEndNode();
	}
}

Intersection* CityObj::getIntersection(int intersectionNum) {
	assert(getInstersectionListSize() < intersectionNum || intersectionNum < 1); // || this->intersections[intersectionNum] == NULL
	return &this->intersections[intersectionNum];
}

Route* CityObj::getPath(Intersection* start, Intersection* end, int* conditions, int fastest) {
}

CityObj::~CityObj() {
	// TODO Auto-generated destructor stub
}

int* CityObj::reverseTrace(int* trace)
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

std::pair<int*, int*>* CityObj::getRoadData(Link* link) {

	std::pair<int*, int*>* roadData;

	if(link->getNumber() > getRoadListSize() || link->isEqual(this->link->finalLink()))
	{
		return roadData;
	}

	roadData->first = this->roadList[link->getNumber()].getSpeedData();
	roadData->second = this->roadList[link->getNumber()].getElevData();

	if(link->getDirection())
	{
		roadData->first = reverseTrace(roadData->first);
		roadData->second = reverseTrace(roadData->second);
	}
	return roadData;
}

bool CityObj::legalRoute(Route* route) {
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

} /* namespace PredictivePowertrain */
