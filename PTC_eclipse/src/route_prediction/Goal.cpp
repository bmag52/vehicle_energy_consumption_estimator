/*
 * Goal.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#include "Goal.h"
#include <functional>
#include <math.h>

namespace RoutePrediction {

Goal::Goal(int destination) {
	this->destination = destination;
}

Goal::Goal(int destination, int bin[], int size) {
	this->destination = destination;
	this->bins = bin;
	this->size = size;
	this->numSeen = 1;
}

Goal::Goal(Goal * other) {
	this->destination = (*other).destination;
	this->size = (*other).size;
	this->bins = new int[size];
	for (int i = 0; i < size; i++) {
		this->bins[i] = (*other).bins[i];
	}
	this->numSeen = (*other).numSeen;
}

bool Goal::isSimilar(Goal * other) {
	if (this->size == (*other).size) {
		for (int i = 0; i < size; i++) {
			if (this->bins[i] != (*other).bins[i]) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool Goal::isEqual(Goal * other) {
	return isSimilar(other) && this->destination == (*other).destination;
}

size_t Goal::getHash() const {
	size_t hash = (size_t) destination;
	int bits;
	for (int i = 0; i < size; i++) {
	  bits = (int) log2(bins[i]);
	  hash = hash << bits;
	  hash += bins[i];
	}
	return hash;
}

Goal::Goal() {
}

Goal::Goal(int destination, int* bin) {
	this->destination = destination;
	this->bins = bin;
	this->size = sizeof(bin) / sizeof(int);
}

void Goal::incrementNumSeen() {
	this->numSeen++;
}

void Goal::setNumSeen(int numSeen) {
	this->numSeen = numSeen;
}

int* Goal::getBins() {
	return this->bins;
}

Goal::~Goal() {
}

int Goal::getNumSeen() {
	return this->numSeen;
}

} /* namespace RoutePrediction */

namespace std {
  template <> struct hash<RoutePrediction::Goal>
  {
    size_t operator()(const RoutePrediction::Goal & x) const
    {
      return x.getHash();
    }
  };
}
