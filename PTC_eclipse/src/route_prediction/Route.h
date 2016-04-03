/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Goal.h"
#include "../driver_prediction/Link.h"
#include "../city/Intersection.h"

using namespace DriverPrediction;
using namespace City;

namespace RoutePrediction {

class Route {
private:
	Link* links;
	Goal* goal;
	Link* error;
	Intersection* intersection;
	bool goalIsIntersection;
	int counter;
public:
	Route();
	Route(Link* links, Goal* goal);
	void addlink(Link* link);
	bool isequal(Route* other);
	int getGoalHash();
	int getLinkSize();
	Link* getEntry(int index);
	void setToIntersection(Intersection* other);
	bool isIntersection();
	bool isEmpty();
	Goal* getGoalPtr();
	Link* getLinksPtr();
	Intersection* getIntersectionPtr();
	Link* getLastLinkPtr();;
	Route copy();
	void removeFirstLink();
	Link nextlink();
};

}
