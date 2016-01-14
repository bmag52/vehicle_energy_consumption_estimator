/*
 * Intersection.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "Intersection.h"

namespace City {


Intersection::Intersection(Road* roadInput, std::string intersectType,
		std::pair<int, int>* coords, int elev, int intersectNum) {
	roads = roadInput;
	interSectionType = IntersectionTypes(intersectType);
	//if length coords is 2 set equal to, obj coordinates= coords else then it is equal to 0,0
	coordinates = coords;
	elevation = elev;
	number = intersectNum;
}

Intersection::~Intersection() {
	delete roads;
	delete coordinates;

}

} /* namespace City */
