/*
 * Bounds.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#include "../data_management/Bounds.h"

namespace PredictivePowertrain {

Bounds::Bounds() {
}

Bounds::Bounds(double maxLat, double maxLon, double minLat, double minLon) {
	this->maxLat = maxLat;
	this->maxLon = maxLon;
	this->minLat = minLat;
	this->minLon = minLon;
}

double Bounds::getMaxLat() {
	return this->maxLat;
}

double Bounds::getMaxLon() {
	return this->maxLon;
}

double Bounds::getMinLat() {
	return this->minLat;
}

double Bounds::getMinLon() {
	return this->minLon;
}

}





