/*
 * Node.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#include "../data_management/Node.h"

namespace PredictivePowertrain {

Node::Node() {
}

Node::Node(double lat, double lon, int ele, long int id) {
	this->lat = lat;
	this->lon = lon;
	this->ele = ele;
	this->id = id;
}

double Node::getLat() {
	return this->lat;
}

double Node::getLon() {
	return this->lon;
}

long int Node::getID() {
	return this->id;
}

int Node::getEle() {
	return this->ele;
}

}
