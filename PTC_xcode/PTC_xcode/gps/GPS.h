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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "../city/City.h"
#include "../city/Road.h"
#include "../city/Intersection.h"

#define EARTH_RADIUS 6371000.0

namespace PredictivePowertrain {
    
class Road;             // forward declaration
class Intersection;     // forward declaration
class City;             // forward declaration

class GPS {
private:
    
	GenericMap<long int, std::pair<double, double>*> tripLog;
    
	long int tripCount;
    double deltaXYTolerance;
    double toRadians(double degrees);
    double toDegrees(double radians);
    double refLat;
    double refLon;
    int fd;
    
    void initializeGPSReader();
    void interpolateTripLog(GenericMap<long int, std::pair<double, double>*>* before, GenericMap<long int, std::pair<double, double>*>* after);

public:
	GPS();
    GPS(double refLat, double refLon);
    ~GPS();
    
    std::pair<double, double>* updateTripLog();
    Road* getCurrentRoad1(City* city);
    Road* getCurrentRoad2(City* city, double lat, double lon);
	GenericMap<long int, std::pair<double, double>*>* getTripLog(bool interpolated);
    float deltaLatLonToXY(double lat1, double lon1, double lat2, double lon2);
    std::pair<double, double>* convertLatLonToXY(double lat, double lon);
    std::pair<double, double>* convertXYToLatLon(double x, double y);
    std::pair<double, double> readGPS();
    bool isOnRoad(Road* road);
    bool isAtIntersection(Intersection* intersection);
    float getDistAlongRoad(Road* road, bool updateTripLog, bool headingIsStart2End);
    bool isHeadingStart2EndOfCurrentRoad(Road* road);
    double getHeadingAngle();
    double getDeltaXYTolerance();
    double boundTheta(double theta);
};

}

#endif /* GPS_GPS_H_ */
