/*
 * Intersection.h
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#ifndef CITY_INTERSECTION_H_
#define CITY_INTERSECTION_H_

namespace City {

class Intersection {
public:
//	Road* roads; //%array of roads that connect to this intersection
//	IntersectionTypes interSectionType;
//	std::pair<int, int>* coordinates;
	int elevation;
	int number;
//	Intersection(Road*, std::string intersectType, std::pair<int, int>*, int, int);
	virtual ~Intersection();
};

} /* namespace City */

#endif /* CITY_INTERSECTION_H_ */
