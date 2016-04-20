/*
 * GPS.cpp
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#include "GPS.h"

namespace PredictivePowertrain {

GPS::GPS() {
}

std::pair<double, double>* GPS::getLatLon() {
}

bool GPS::isOnRoad(Road* road) {
}

void GPS::updateTripLog() {
	std::pair<double, double>* latLon = this->getLatLon();
	this->tripLog.addEntry(latLon->first, latLon->second);
}

GenericMap<double, double>* GPS::getTripLog() {
	return &this->tripLog;
}

}
