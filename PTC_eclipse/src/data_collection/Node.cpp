/*
 * Node.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#include "Node.h"

namespace PredictivePowertrain {

Node::Node() {
}

Node::Node(double lat, double lon, int id) {
	this->lat = lat;
	this->lon = lon;
	this->id = id;
}

double Node::getLat() {
	return this->lat;
}

double Node::getLon() {
	return this->lon;
}

int Node::getID() {
	return this->id;
}

double Node::getEle() {
	return this->ele;
}

}
