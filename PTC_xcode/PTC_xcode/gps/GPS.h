/*
 * GPS.h
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#ifndef GPS_GPS_H_
#define GPS_GPS_H_

#include <utility>
#include <cmath>
#include <math.h>
#include "../city/Road.h"

namespace PredictivePowertrain {

class GPS {
private:
	GenericMap<long int, std::pair<double, double>*> tripLog;
	long int tripCount;
    float toRadians(float degrees);

public:
	GPS();
	static std::pair<double, double>* getLatLon();
	static bool isOnRoad(Road* road);
	void updateTripLog();
	GenericMap<long int, std::pair<double, double>*>* getTripLog();
    float deltaLatLonToXY(float lat1, float lon1, float lat2, float lon2);
};

}



#endif /* GPS_GPS_H_ */
