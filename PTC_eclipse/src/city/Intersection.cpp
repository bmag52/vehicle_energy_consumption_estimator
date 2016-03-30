/*
 * Intersection.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "Intersection.h"

#include <algorithm>

namespace PredictivePowertrain {

Intersection::Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum) {
	this->roads = roadInput;
//	this->interSectionType = IntersectionTypes(intersectType);
	this->lat = lat;
	this->lon = lon;
	this->elevation = elev;
	this->number = intersectNum;
}

Intersection::~Intersection() {
}

Intersection::Intersection() {
}

int Intersection::getNumber() {
	return this->number;
}

double Intersection::getElevation() {
	return this->elevation;
}

double Intersection::getLat() {
	return this->lat;
}

double Intersection::getLon() {
	return this->lon;
}

Road* Intersection::getRoads() {
	return this->roads;
}

void Intersection::addRoad(Road* road, int roadDir) {
	this->roadCount++;
	Road* newRoads = new Road[this->roadCount];
	if(roadDir == 0)
	{
		road->setEndNumber(this->number);
	} else {
		road->setStartNumber(this->number);
	}

	newRoads[0] = *road;
	for(int i = 1; i < this->roadCount; i++)
	{
		newRoads[i] = this->roads[i-1];
	}

	free(this->roads);
	this->roads = newRoads;
}


Link* Intersection::getOutgoingLinks() {
	Link* outgoingLinks = new Link[this->roadCount];
	for(int i = 0; i < this->roadCount; i++)
	{
		outgoingLinks[i] = *(linkFromRoad(&this->roads[i], this));
	}
	return outgoingLinks;
}

int Intersection::getNextIntersectionNumber(Road* road) {
	for(int i = 0; i < this->roadCount; i++)
	{
		if(road->getRoadID() == this->roads[i].getRoadID())
		{
			if(this->number == this->roads[i].getStartNumber())
			{
				return this->roads[i].getEndNumber();
			} else {
				return this->roads[i].getStartNumber();
			}
		}
	}
	return -1;
}

int* Intersection::getAdjacentIntersectionNumbers() {
	int* adjIntNums;
	int adjIntNumCount = 0;

	for(int i = 0; this->roadCount; i++)
	{
		int adjIntNum = getNextIntersectionNumber(&this->roads[i]);
		bool alreadyCounted = false;

		for(int j = 0; j < adjIntNumCount; j++)
		{
			if(adjIntNums[i] == adjIntNum)
			{
				alreadyCounted = true;
				break;
			}
		}

		if(!alreadyCounted)
		{
			adjIntNumCount++;
			int* newAdjIntNums = new int[adjIntNumCount];

			newAdjIntNums[0] = adjIntNum;
			for(int j = 1; j < adjIntNumCount; j++)
			{
				newAdjIntNums[j] = adjIntNums[i-1];
			}

			free(adjIntNums);
			adjIntNums = newAdjIntNums;
		}
	}
	return adjIntNums;
}

} /* namespace PredictivePowertrain */

