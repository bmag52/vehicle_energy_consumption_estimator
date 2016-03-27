/*
 * Link.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#include "Link.h"

namespace DriverPrediction {

Link::Link() {
	this->link_number = 0;
	this->link_direction = 0;

}

Link::Link(int direction, int linkNumber) {
	this->link_direction = direction;
	this->link_number = linkNumber;
}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

Link* Link::copy(int direction, int linkNumber) {
	Link link(direction, linkNumber);
	return &link;
}

bool Link::isEqual(Link* other) {
	if ((*other).link_direction == this->link_direction && (*other).link_number == this->link_number) {
		return true;
	} else {
		return false;
	}
}

int Link::getNumber() {
	return this->link_number;
}

int Link::getDirection() {
	return this->link_direction;
}

//Takes in hash number and returns new link corresponding to hash
Link* Link::newLinkFromHash(int hash) {
	Link link(hash % 2, hash / 2);
	return &link;
}

int Link::getHash() {
	return 2 * this->link_direction + this->link_number;
}

// %represents the link at the end of a route, when the vehicle stops
Link* Link::finalLink() {
	Link link(0, 0);
	return &link;
}

/* Link Link::link_from_road(City::Road road, City::Intersection intersection) {
	int link_num = road.road_id;
	road.end_node = intersection.number;
	int link_dir = road.end_node;
	return Link(link_dir, link_num);
}*/

} /* namespace DriverPrediction */
