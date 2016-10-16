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
    this->initialize();
}

Link::Link(int direction, long int linkNumber) {
    this->link_number = linkNumber;
	this->link_direction = direction;
    this->initialize();
}
    
void Link::initialize()
{
    this->WtsA = NULL;
    this->yHidA = NULL;
    this->yInHidA = NULL;
    
    this->WtsB = NULL;
    this->yHidB = NULL;
    this->yInHidB = NULL;
    
    this->numNNLayers = 0;
}

Link::~Link() {
	// TODO Auto-generated destructor stub
}

Link* Link::copy(int direction, long int linkNumber) {
	Link* link = new Link(direction, linkNumber);
	return link;
}

bool Link::isEqual(Link* other) {
    return other->link_direction == this->link_direction && other->link_number == this->link_number;
}

long int Link::getNumber() {
	return this->link_number;
}

int Link::getDirection() {
	return this->link_direction;
}

Link* Link::newLinkFromHash(long int hash) {
	Link* link = new Link(hash % 2, hash / 2);
	return link;
}

long int Link::getHash() {
	return 2 * this->link_direction + this->link_number;
}

Link* Link::finalLink() {
	Link* link = new Link(-1, -1);
	return link;
}

bool Link::isFinalLink()
{
    return this->link_direction == -1 && this->link_number == -1;
}

Link* Link::linkFromRoad(Road* road, Intersection* intersection) {
	long int linkNum = road->getRoadID();
	int linkDir = road->getEndIntersection()->getIntersectionID() == intersection->getIntersectionID();
	Link* link = new Link(linkDir, linkNum);
	return link;
}
    
void Link::setWeights(Eigen::MatrixXd* wts, Eigen::MatrixXd* yHid, Eigen::MatrixXd* yInHid, int direction)
{
    if(direction == 1)
    {
        this->WtsA = wts;
        this->yHidA = yHid;
        this->yInHidA = yInHid;
    }
    else
    {
        this->WtsB = wts;
        this->yHidB = yHid;
        this->yInHidB = yInHid;
    }
}
    
std::list<Eigen::MatrixXd*>* Link::getWeights(int direction)
{
    std::list<Eigen::MatrixXd*>* returnList = new std::list<Eigen::MatrixXd*>();
    if(direction == 1)
    {
        returnList->push_front(this->WtsA);
        returnList->push_front(this->yHidA);
        returnList->push_front(this->yInHidA);
    }
    else
    {
        returnList->push_front(this->WtsB);
        returnList->push_front(this->yHidB);
        returnList->push_front(this->yInHidB);
    }
    return returnList;
}
    
void Link::setNumNNLayers(int num)
{
    this->numNNLayers = num;
}

int Link::getNumNNLayers()
{
    return this->numNNLayers;
}
    
} /* namespace PredictivePowertrain */
