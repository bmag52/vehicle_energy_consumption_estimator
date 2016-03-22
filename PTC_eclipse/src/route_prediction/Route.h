/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#include "Goal.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace RoutePrediction {

class Route {
private:
	Link links[];
	Goal goal;
	int counter;
public:
	Route();
	void addlink(Link link);
	bool isequal(Route other);
	Route copy();
	Link nextlink();
};

}
