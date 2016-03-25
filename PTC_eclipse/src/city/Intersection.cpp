/*
 * Intersection.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "Intersection.h"

namespace City {


Intersection::Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum) {
	this->roads = roadInput;
//	this->interSectionType = IntersectionTypes(intersectType);
	this->lat = lat;
	this->lon = lon;
	this->elevation = elev;
	this->number = intersectNum;
}

Intersection::~Intersection() {
}

void Intersection::getOutgoingLinks(Link* link) {
	// TODO getOutgoingLinks
}

} /* namespace City */
