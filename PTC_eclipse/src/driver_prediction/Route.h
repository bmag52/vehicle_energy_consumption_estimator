/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */
#include "../route_prediction/Link.h"
#include "Goal.h"


namespace DriverPrediction {

class Route {
private:
	Link links[];
	Goal goal;
	int counter;
public:
	Route(Link links[], Goal& goal);
	void addlink(Link link);
	bool isequal(Route& other);
	Route copy();
	Link nextlink();
};

}
