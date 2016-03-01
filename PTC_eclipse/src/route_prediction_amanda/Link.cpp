/*
 * Link.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#include "Link.h"


namespace DriverPrediction {

Link::Link() {
	link_number = 0;
	link_direction = 0;

}

Link::Link(int direction, int linkNumber) {
	link_direction = direction;
	link_number = linkNumber;
}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

Link Link::copy(int direction, int linkNumber) {
	return Link(direction, linkNumber);
}



int Link::get_hash(Link link) {
	return 2 * link_direction + link_number;
}

bool Link::isEqual(Link other) {
	if (other.link_direction == link_direction && other.link_number == link_number) {
		return true;
	} else {
		return false;
	}
}

int Link::getNumber() {
	return link_number;
}

int Link::getDirection() {
	return link_direction;
}


Link Link::new_link_from_hash(int hash) {
	//std::string binaryStuff = convertToBinary(hash);
	///???
	//do we need this method?
	//MATLAB CODE
	/**
	 * hash_bin = dec2bin(hash);
            dir = bin2dec(hash_bin(end));
            link_num = bin2dec(hash_bin(1:(end-1)));
            new_link = Link(link_num, dir);
	 */
}

// %represents the link at the end of a route, when the vehicle stopes
//
Link Link::final_link() {
	return Link(0, 0);
}

//Link Link::link_from_road(City::Road road, City::Intersection intersection) {
//	int link_num = road.road_id;
//	road.end_node = intersection.number;
//	int link_dir = road.end_node;
//	return Link(link_dir, link_num);
//}

} /* namespace DriverPrediction */
