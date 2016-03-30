/*
 * CityObj.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "City.h"

namespace PredictivePowertrain {

CityObj::CityObj() {
	// TODO Auto-generated constructor stub
}

int CityObj::getRoadListSize() {
	return sizeof(*this->roadList)/sizeof(Road);
}

int CityObj::getInstersectionListSize() {
	return sizeof(*this->roadList)/sizeof(Road);
}

Link* CityObj::getNextLinks(Link* link) {

}

Intersection* CityObj::getIntersectionFromLink(Link* link) {
	// TODO getIntersectionFromLInk
}

CityObj::~CityObj() {
	// TODO Auto-generated destructor stub
}

} /* namespace PredictivePowertrain */
