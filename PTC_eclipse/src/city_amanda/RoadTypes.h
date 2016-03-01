/*
 * RoadTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROADTYPES_H_
#define ROADTYPES_H_

namespace City {

class RoadTypes {
public:
//	std::string color;
	int max_speed;
	int min_speed;
	int max_distance;
	int min_distance;
//	std::string toString;
//	RoadTypes(std::string);
	virtual ~RoadTypes();
};

} /* namespace City */

#endif /* ROADTYPES_H_ */
