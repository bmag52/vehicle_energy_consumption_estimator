/*
 * GPS.h
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#ifndef GPS_GPS_H_
#define GPS_GPS_H_

#include <utility>
#include "../city/Road.h"

namespace PredictivePowertrain {

class GPS {
private:
	double lat;
	double lon;

public:
	GPS();
	std::pair<double, double>* getLatLon();
	bool isOnRoad(Road* road);
};

}



#endif /* GPS_GPS_H_ */
