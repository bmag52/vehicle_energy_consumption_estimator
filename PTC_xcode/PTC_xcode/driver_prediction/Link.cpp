/*
 * Link.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#include "Link.h"

namespace PredictivePowertrain {

Link::Link() {
	this->link_number = 0;
	this->link_direction = 0;

}

Link::Link(int direction, int linkNumber) {
    this->link_number = linkNumber;
	this->link_direction = direction;
}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

Link* Link::copy(int direction, int linkNumber) {
	Link* link = new Link(direction, linkNumber);
	return link;
}

bool Link::isEqual(Link* other) {
    return other->link_direction == this->link_direction && other->link_number == this->link_number;
}

int Link::getNumber() {
	return this->link_number;
}

int Link::getDirection() {
	return this->link_direction;
}

Link* Link::newLinkFromHash(int hash) {
	Link* link = new Link(hash % 2, hash / 2);
	return link;
}

int Link::getHash() {
	return 2 * this->link_direction + this->link_number;
}

Link* Link::finalLink() {
	Link* link = new Link(0, 0);
	return link;
}

bool Link::isFinalLink()
{
    return this->link_direction == 0 && this->link_number == 0;
}

Link* Link::linkFromRoad(Road* road, Intersection* intersection) {
	int linkNum = road->getRoadID();
	int linkDir = road->getEndIntersection()->getIntersectionID() == intersection->getIntersectionID();
	Link* link = new Link(linkDir, linkNum);
	return link;
}

} /* namespace PredictivePowertrain */
