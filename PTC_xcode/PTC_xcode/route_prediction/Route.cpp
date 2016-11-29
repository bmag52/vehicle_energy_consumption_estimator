/*
 * Route.cpp
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Route.h"

namespace PredictivePowertrain {
    
Route::~Route()
{
    delete(this->links);
    delete(this->intersection);
}

Route::Route()
{
	this->linkCount = 0;
    this->links = new GenericMap<long int, Link*>();
    this->intersection = new Intersection();
}

Route::Route(GenericMap<long int, Link*>* links, Goal* goal) {
	this->links = links;
	this->goal = goal;
	this->linkCount = links->getSize();
	Link error(NULL, -1);
	this->error = &error;
    this->intersection = new Intersection();
}

// adds new link to end of route
void Route::addLink(Link* link) {
	this->links->addEntry(this->linkCount, link);
    this->linkCount++;
}

// checks if the route is equal to the route passed in
bool Route::isEqual(Route * other)
{
	if(this->getLinkSize() == other->getLinkSize()) {
		GenericMap<long int, Link*>* otherLinks = other->getLinks();

        for(int i = 0; i < this->getLinkSize(); i++)
        {
            Link* thisLink = this->links->getEntry(i);
            Link* otherLink = otherLinks->getEntry(i);
            
            if(!thisLink->isEqual(otherLink))
            {
                return false;
            }
        }
        
		return true;
	} else {
		return false;
	}
}

Route* Route::copy() {
	Goal* newGoal = new Goal(this->goal);
	GenericMap<long int, Link*>* newLinks = this->links->copy();
	Route* route = new Route(newLinks, newGoal);
	return route;
}

long int Route::getGoalHash() {
	return this->goal->getHash();
}

Goal* Route::getGoal() {
	return this->goal;
}
    
void Route::assignGoal(Goal* goal)
{
    this->goal = goal;
}

int Route::getLinkSize() {
	return this->links->getSize();
}

GenericMap<long int, Link*>* Route::getLinks() {
	return this->links;
}

void Route::setToIntersection(Intersection* other) {
	this->intersection = other;
	this->routeIsIntersection = true;
}
    
void Route::setToRoute() {
    this->intersection = NULL;
    this->routeIsIntersection = false;
}

bool Route::isIntersection() {
	return this->routeIsIntersection;
}

Intersection* Route::getIntersection() {
	return this->intersection;
}

Link* Route::getLastLink() {
	return this->links->getEntry(this->linkCount - 1);
}

bool Route::isEmpty() {
	return this->links->getSize() == 0;
}

Link* Route::getEntry(int index) {
	if(index > this->linkCount - 1)
	{
		return this->error;
	}
	return this->links->getEntry(index);
}

void Route::removeFirstLink() {
	if(this->links->getSize() > 0)
	{
        this->links->indexErase(0);
	}
    this->linkCount--;
}
    
void Route::saveRoute2CSV(FILE* file, City* city, bool includeheaderAndCloseFile)
{
    if(includeheaderAndCloseFile)
    {
        fprintf(file, "name, description, color, latitude, longitude\n");
    }
    
    this->links->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = this->links->nextEntry();
    while(nextLink != NULL && !nextLink->value->isFinalLink())
    {
        
        Road* currRoad = city->getRoads()->getEntry(nextLink->value->getNumber());
        
        // intersections
        for(int i = 0; i < 2; i++)
        {
            Intersection* intersection = currRoad->getStartIntersection();
            if(i == 1)
            {
                intersection = currRoad->getEndIntersection();
            }
            
            // intersections
            fprintf(file, "%ld,", intersection->getIntersectionID());
            fprintf(file, "Lat & Lon: %.12f %.12f,", intersection->getLat(), intersection->getLon());
            fprintf(file, "blue,");
            fprintf(file, "%.12f,%.12f\n", intersection->getLat(), intersection->getLon());
        }
        
        currRoad->getNodes()->initializeCounter();
        GenericEntry<long int, Node*>* nextNode = currRoad->getNodes()->nextEntry();
        
        // skip first node
        nextNode = currRoad->getNodes()->nextEntry();
        int nodeCount = 1;
        
        while(nodeCount < currRoad->getNodes()->getSize() - 1)
        {
            // road nodes
            fprintf(file, "%ld,", currRoad->getRoadID());
            fprintf(file, "Lat & Lon: %.12f %.12f,", nextNode->value->getLat(), nextNode->value->getLon());
            fprintf(file, "red,");
            fprintf(file, "%.12f,%.12f\n", nextNode->value->getLat(), nextNode->value->getLon());
            
            nodeCount++;
            
            nextNode = currRoad->getNodes()->nextEntry();
        }
        delete(nextNode);
        
        nextLink = this->links->nextEntry();
    }
    delete(nextLink);
    
    if(includeheaderAndCloseFile)
    {
        fclose(file);
    }
}
    
void Route::printLinks()
{
    this->links->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = this->links->nextEntry();
    while(nextLink != NULL)
    {
        std::cout << nextLink->value->getNumber() << " | ";
        nextLink = this->links->nextEntry();
    }
    std::cout << std::endl;
    
    delete(nextLink);
}
    
void Route::replaceLinks(GenericMap<long int, Link*>* newLinks)
{
    assert(newLinks->getSize() == this->links->getSize());
    delete(this->links);
    this->links = newLinks;
}
    
void Route::addLinkToFront(Link* frontLink)
{
    GenericMap<long int, Link*>* newLinks = new GenericMap<long int, Link*>();
    newLinks->addEntry(0, frontLink);
    
    this->links->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = this->links->nextEntry();
    while(nextLink != NULL)
    {
        newLinks->addEntry(nextLink->key + 1, nextLink->value);
        nextLink = this->links->nextEntry();
    }
    delete(nextLink);
    
    delete(this->links);
    this->links = newLinks;
    
    this->linkCount++;
}

}
