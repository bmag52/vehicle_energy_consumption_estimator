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
		road->setEndNode(this);
	} else {
		road->setStartNode(this);
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
	Link* outGoingLinks = new Link[this->roadCount];
	for(int i = 0; i < this->roadCount; i++)
	{
		outGoingLinks[i] = *(outGoingLinks->linkFromRoad(&this->roads[i], this));
	}
	return outGoingLinks;
}

int Intersection::getRoadCount() {
	return this->roadCount;
}

Intersection* Intersection::getNextIntersection(Road* road) {
	for(int i = 0; i < this->roadCount; i++)
	{
		if(road->getRoadID() == this->roads[i].getRoadID())
		{
			if(this->number == this->roads[i].getStartNode()->getNumber())
			{
				return this->roads[i].getEndNode();
			} else {
				return this->roads[i].getStartNode();
			}
		}
	}
	return NULL;
}

Intersection* Intersection::getAdjacentIntersection() {
	Intersection* adjInts;
	int adjIntCount = 0;

	for(int i = 0; this->roadCount; i++)
	{
		Intersection* adjInt = getNextIntersection(&this->roads[i]);
		bool alreadyCounted = false;

		for(int j = 0; j < adjIntCount; j++)
		{
			if(adjInts[i].getNumber() == adjInt->getNumber())
			{
				alreadyCounted = true;
				break;
			}
		}

		if(!alreadyCounted)
		{
			adjIntCount++;
			Intersection* newAdjInts = new Intersection[adjIntCount];

			newAdjInts[0] = *adjInt;
			for(int j = 1; j < adjIntCount; j++)
			{
				newAdjInts[j] = adjInts[i-1];
			}

			free(adjInts);
			adjInts = newAdjInts;
		}
	}
	return adjInts;
}

} /* namespace PredictivePowertrain */

