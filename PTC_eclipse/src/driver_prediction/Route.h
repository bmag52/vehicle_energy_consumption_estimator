/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

//#ifndef DRIVER_PREDICTION_ROUTE_H_
//#define DRIVER_PREDICTION_ROUTE_H_

#include "../route_prediction/Link.h"
#include "../route_prediction/Goal.h"


namespace InnovationModel {

class Route {
private:
	Link links[];
	Goal goal;
	int counter;
public:
	Route(Link links[], Goal& goal);
	virtual ~Route();
	void addlink(Link link);
	bool isequal(Route& other);
	Route copy();
	Link nextlink();
};

}
//#endif
