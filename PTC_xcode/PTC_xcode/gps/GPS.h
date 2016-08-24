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

#define EARTH_RADIUS 6371000.0

namespace PredictivePowertrain {

class GPS {
private:
	GenericMap<long int, std::pair<double, double>*> tripLog;
	long int tripCount;
    double toRadians(double degrees);
    double toDegrees(double radians);
    double refLat;
    double refLon;

public:
	GPS();
    GPS(double refLat, double refLon);
	static std::pair<double, double>* getLatLon();
	static bool isOnRoad(Road* road);
	void updateTripLog();
	GenericMap<long int, std::pair<double, double>*>* getTripLog();
    float deltaLatLonToXY(double lat1, double lon1, double lat2, double lon2);
    std::pair<double, double>* convertLatLonToXY(double lat, double lon);
    std::pair<double, double>* convertXYToLatLon(double x, double y);
};

}



#endif /* GPS_GPS_H_ */
