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

}
