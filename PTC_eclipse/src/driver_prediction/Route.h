/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_H_
#define ROUTE_H_

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
	~Route();
	void addlink(Link link);
	bool isequal(Route& other);
	Route copy();
	Link nextlink();
};

}
#endif
