/*
 * Bounds.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#include "Bounds.h"

namespace PredictivePowertrain {

Bounds::Bounds()
{
}

Bounds::Bounds(double maxLat, double maxLon, double minLat, double minLon) {
	this->maxLat = maxLat;
	this->maxLon = maxLon;
	this->minLat = minLat;
	this->minLon = minLon;
	this->id = -1;
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

void Bounds::assignID(int id) {
	this->id = id;
}

int Bounds::getID() {
	return this->id;
}
    
std::pair<double, double>* Bounds::getMidLatLon()
{
    return new std::pair<double, double>(this->maxLat - this->minLat, this->maxLon - this->minLon);
}

}


