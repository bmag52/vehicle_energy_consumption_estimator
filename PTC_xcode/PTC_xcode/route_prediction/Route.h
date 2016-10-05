/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_H_
#define ROUTE_H_

#include "Goal.h"
#include "../driver_prediction/Link.h"
#include "../city/Intersection.h"

namespace PredictivePowertrain {

class Route {
private:
	GenericMap<int, Link*>* links;
	Goal* goal;
	Link* error;
	Intersection* intersection;
	bool routeIsIntersection;
	int linkCount;
public:
	Route();
	Route(GenericMap<int, Link*>* links, Goal* goal);
    void printLinks();
    virtual ~Route();
    void addLink(Link* link);
	bool isEqual(Route* other);
	int getGoalHash();
	int getLinkSize();
    void assignGoal(Goal* goal);
	Link* getEntry(int index);
	void setToIntersection(Intersection* other);
    void setToRoute();
	bool isIntersection();
	bool isEmpty();
	Goal* getGoal();
	GenericMap<int, Link*>* getLinks();
	Intersection* getIntersection();
	Link* getLastLink();;
	Route* copy();
	void removeFirstLink();
};

}

#endif