/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_H_
#define ROUTE_H_

#include <iostream>
#include <fstream>

#include "Goal.h"
#include "../driver_prediction/Link.h"
#include "../city/Intersection.h"
#include "../city/City.h"

namespace PredictivePowertrain {
    
class City;         // forward delcaration

class Route {
private:
	GenericMap<long int, Link*>* links;
	Goal* goal;
	Link* error;
	Intersection* intersection;
	bool routeIsIntersection;
	int linkCount;
public:
	Route();
	Route(GenericMap<long int, Link*>* links, Goal* goal);
    void printLinks();
    virtual ~Route();
    void addLink(Link* link);
	bool isEqual(Route* other);
	long int getGoalHash();
	int getLinkSize();
    void assignGoal(Goal* goal);
	Link* getEntry(int index);
	void setToIntersection(Intersection* other);
    void setToRoute();
	bool isIntersection();
	bool isEmpty();
	Goal* getGoal();
	GenericMap<long int, Link*>* getLinks();
	Intersection* getIntersection();
	Link* getLastLink();;
	Route* copy();
	void removeFirstLink();
    void addLinkToFront(Link* frontLink);
    void replaceLinks(GenericMap<long int, Link*>* newLinks);
    void saveRoute2CSV(FILE* file, City* city, bool includeheaderAndCloseFile);
};

}

#endif
