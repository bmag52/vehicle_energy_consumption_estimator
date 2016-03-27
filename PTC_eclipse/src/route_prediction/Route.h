/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */
#include "Link.h"
#include "../route_prediction/Goal.h"


namespace DriverPrediction {

using namespace InnovationModel;

#include "Goal.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace RoutePrediction {

class Route {
private:
	Link* links;
	Goal* goal;
	int counter;
public:
	Route();
<<<<<<< HEAD
	Route(Link links[], Goal& goal);
	void addlink(Link link);
	bool isequal(Route& other);
=======
	Route(Link* links, Goal* goal);
	void addlink(Link* link);
	bool isequal(Route other);
	int getGoalHash();
	int getLinkSize();
	Goal* getGoalPtr();
	Link* getLinksPtr();
>>>>>>> master
	Route copy();
	void initiateiterator();
	Link nextlink();
};

}
