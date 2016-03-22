/*
 * Route.h
 *
 *  Created on: Mar 6, 2016
 *      Author: vagrant
 */
#include "Link.h"
#include "../route_prediction/Goal.h"


<<<<<<< HEAD:PTC_eclipse/src/driver_prediction/Route.h
namespace DriverPrediction {

using namespace InnovationModel;
=======
#include "Goal.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace RoutePrediction {
>>>>>>> master:PTC_eclipse/src/route_prediction/Route.h

class Route {
private:
	Link links[];
	Goal goal;
	int counter;
public:
<<<<<<< HEAD:PTC_eclipse/src/driver_prediction/Route.h
	Route(Link links[], Goal& goal);
	void addlink(Link link);
	bool isequal(Route& other);
=======
	Route();
	void addlink(Link link);
	bool isequal(Route other);
>>>>>>> master:PTC_eclipse/src/route_prediction/Route.h
	Route copy();
	Link nextlink();
};

}
