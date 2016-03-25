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

Link* Link::copy(int direction, int linkNumber) {
	Link link(direction, linkNumber);
	return &link;
}


int Link::getHash(Link* link) {
	return 2 * link_direction + link_number;
}

bool Link::isEqual(Link* other) {
	if ((*other).link_direction == link_direction && (*other).link_number == link_number) {
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

//Takes in hash number and returns new link corresponding to hash
Link* Link::newLinkFromHash(int hash) {
	Link link(hash % 2, hash / 2);
	return &link;
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
