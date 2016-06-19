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

Node::Node(float lat, float lon, int ele, long int id) {
	this->lat = lat;
	this->lon = lon;
	this->ele = ele;
	this->id = id;
}

float Node::getLat() {
	return this->lat;
}

float Node::getLon() {
	return this->lon;
}

long int Node::getID() {
	return this->id;
}

int Node::getEle() {
	return this->ele;
}

}
