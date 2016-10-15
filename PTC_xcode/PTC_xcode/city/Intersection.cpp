/*
 * Intersection.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "Intersection.h"

#include <algorithm>

namespace PredictivePowertrain {

Intersection::Intersection(GenericMap<long int, Road*>* roads, double lat, double lon, int elev, long int intersectNum) {
	this->roads = roads;
//	this->interSectionType = IntersectionTypes(intersectType);
	this->lat = lat;
	this->lon = lon;
	this->elevation = elev;
	this->ID = intersectNum;
}

Intersection::~Intersection() {
    delete(this->roads);
}

Intersection::Intersection() {
    this->roads = new GenericMap<long int, Road*>();
}
    
void Intersection::assignID(long int ID)
{
    this->ID = ID;
}

long int Intersection::getIntersectionID() {
	return this->ID;
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

GenericMap<long int, Road*>* Intersection::getRoads() {
	return this->roads;
}

void Intersection::addRoad(Road* road, int roadDir) {
	if(roadDir == 0)
	{
		road->setEndIntersection(this);
	} else {
		road->setStartIntersection(this);
	}

	this->roads->addEntry(road->getRoadID(), road);
}
    
void Intersection::replaceRoads(GenericMap<long int, Road*>* roads)
{
    delete(this->roads);
    this->roads = roads;
}

GenericMap<int, Link*>* Intersection::getOutgoingLinks() {
	GenericMap<int, Link*>* outGoingLinks = new GenericMap<int, Link*>();

	int linkCount = 0;
	this->roads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = this->roads->nextEntry();
	while(nextRoad != NULL)
	{
		Link* newLink = this->link->linkFromRoad(nextRoad->value, this);
		outGoingLinks->addEntry(linkCount, newLink);
        linkCount++;
        
		nextRoad = this->roads->nextEntry();

	}
	delete(nextRoad);
	return outGoingLinks;
}

int Intersection::getRoadCount() {
	return this->roads->getSize();
}

Intersection* Intersection::getNextIntersection(Road* road) {
	this->roads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = this->roads->nextEntry();
	while(nextRoad != NULL)
	{
		if(road->getRoadID() == nextRoad->value->getRoadID())
		{
			if(this->ID == nextRoad->value->getStartIntersection()->getIntersectionID())
			{
				delete(nextRoad);
				return nextRoad->value->getEndIntersection();
			} else {
				delete(nextRoad);
				return nextRoad->value->getStartIntersection();
			}
		}
		nextRoad = this->roads->nextEntry();
	}
	delete(nextRoad);
	return NULL;
}

GenericMap<int, Intersection*>* Intersection::getAdjacentIntersection() {
	GenericMap<int, Intersection*>* adjInts = new GenericMap<int, Intersection*>();
	int adjIntCount = 0;

	this->roads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = this->roads->nextEntry();
	while(nextRoad != NULL)
	{
		Intersection* adjInt = getNextIntersection(nextRoad->value);
		bool alreadyCounted = false;

		adjInts->initializeCounter();
		GenericEntry<int, Intersection*>* nextInt = adjInts->nextEntry();
		while(nextInt != NULL)
		{
			if(nextInt->value->getIntersectionID() == adjInt->getIntersectionID())
			{
				alreadyCounted = true;
				break;
			}
			nextInt = adjInts->nextEntry();
		}

		if(!alreadyCounted)
		{
			adjInts->addEntry(adjInt->getIntersectionID(), adjInt);
		}
		nextRoad = this->roads->nextEntry();
	}
	return adjInts;
}

void Intersection::setBoundsID(int ID) {
	this->boundsID = ID;
}

int Intersection::getBoudsID() {
	return this->boundsID;
}

} /* namespace PredictivePowertrain */


