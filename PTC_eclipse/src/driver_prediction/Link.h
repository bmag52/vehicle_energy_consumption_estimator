/*
 * Link.h
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#ifndef LINK_H_
#define LINK_H_

namespace DriverPrediction {

class Link {
public:
	int link_number;
	int link_direction;
	Link();
	Link(int, int);
	virtual ~Link();
	Link copy(int, int);
	int get_hash(Link);
	bool isEqual(Link);
	int getNumber();
	int getDirection();
//	static std::string convertToBinary(int);
	static Link new_link_from_hash(int);
	static Link final_link();
//	static Link link_from_road(City::Road, City::Intersection);
//	static Link link_from_road(road, current_intersection);

};


} /* namespace DriverPrediction */

#endif /* LINK_H_ */
