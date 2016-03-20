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
