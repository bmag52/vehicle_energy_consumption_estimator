/*
 * CitySection.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef CITY_CITYSECTION_H_
#define CITY_CITYSECTION_H_

namespace City {
//city_section describes a square section of a city
class CitySection {
public:
//	RoadTypes roadType;
	int x_dim;
	int y_dim;
	int* elevation;
	int* pairing; //what are pairings?
//	std::pair<int, int>* coordinates;
//	IntersectionTypes* intersections;
//	CitySection(std::string, int, int);

	CitySection();
	virtual ~CitySection();
};

} /* namespace City */

#endif /* CITY_CITYSECTION_H_ */
