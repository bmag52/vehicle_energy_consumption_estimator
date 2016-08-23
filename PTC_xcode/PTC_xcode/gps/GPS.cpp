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
    
float GPS::deltaLatLonToXY(float lat1, float lon1, float lat2, float lon2)
{
    double earthRadius = 6371000; //meters
    double dLat = toRadians(lat2-lat1);
    double dLon = toRadians(lon2-lon1);
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
    std::cos(toRadians(lat1)) * std::cos(toRadians(lat2)) *
    std::sin(dLon/2) * std::sin(dLon/2);
    
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    float dist = (float) (earthRadius * c);
    
    return dist;
}
    
float GPS::toRadians(float degrees)
{
    return degrees / 180.0 * M_PI;
}
    
}
