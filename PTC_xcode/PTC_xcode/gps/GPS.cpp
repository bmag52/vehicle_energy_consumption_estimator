/*
 * GPS.cpp
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#include "GPS.h"

namespace PredictivePowertrain {

GPS::GPS() {
	this->tripCount = 0;
}
    
GPS::GPS(double refLat, double refLon)
{
    this->refLat = refLat;
    this->refLon = refLon;
}

std::pair<double, double>* GPS::getLatLon() {
	double lat, lon;


	return new std::pair<double, double>(lat, lon);
}

bool GPS::isOnRoad(Road* road) {
    // TODO
    return NULL;
}

void GPS::updateTripLog() {
	std::pair<double, double>* latLon = this->getLatLon();
	this->tripLog.addEntry(this->tripCount++, latLon);
}

GenericMap<long int, std::pair<double, double>*>* GPS::getTripLog() {
	return &this->tripLog;
}
    
float GPS::deltaLatLonToXY(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = toRadians(lat2-lat1);
    double dLon = toRadians(lon2-lon1);
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
    std::cos(toRadians(lat1)) * std::cos(toRadians(lat2)) *
    std::sin(dLon/2) * std::sin(dLon/2);
    
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    float dist = (float) (EARTH_RADIUS * c);
    
    return dist;
}
    
std::pair<double, double>* GPS::convertLatLonToXY(double lat, double lon)
{
    float dist = deltaLatLonToXY(lat, lon, this->refLat, this->refLon);
    double angle = std::atan2(lat - this->refLat, lon - this->refLon);
    return new std::pair<double, double>(dist * std::sin(angle), dist * std::cos(angle));
}
    
std::pair<double, double>* GPS::convertXYToLatLon(double x, double y)
{
    double latDelta = y/EARTH_RADIUS;
    double lonDelta = x/(EARTH_RADIUS*std::cos(this->refLat));
    
    return new std::pair<double, double>(this->refLat+toDegrees(latDelta), this->refLon+toDegrees(lonDelta));
}
    
double GPS::toDegrees(double radians)
{
    return radians / (2* M_PI) * 360.0;
}
    
double GPS::toRadians(double degrees)
{
    return degrees / 180.0 * M_PI;
}
    
}
