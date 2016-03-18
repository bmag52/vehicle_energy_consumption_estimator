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

Goal::Goal(int destination, int bin[], int size) {
	this->destination = destination;
	this->bins = bin;
	this->size = size;
}

Goal::Goal(Goal& other) {
	this->destination = other.destination;
	this->size = other.size;
	this->bins = new int[size];
	for (int i = 0; i < size; i++) {
		this->bins[i] = other.bins[i];
	}
}

bool Goal::issimilar(Goal& other) {
	if (this->size == other.size) {
		for (int i = 0; i < size; i++) {
			if (this->bins[i] != other.bins[i]) {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

bool Goal::isequal(Goal& other) {
	return issimilar(other) && this->destination == other.destination;
}

size_t Goal::get_hash() const {
	size_t hash = (size_t) destination;
	int bits;
	for (int i = 0; i < size; i++) {
	  bits = (int) log2(bins[i]);
	  hash = hash << bits;
	  hash += bins[i];
	}
	return hash;
}

Goal::~Goal() {
}

} /* namespace RoutePrediction */

namespace std {
  template <> struct hash<RoutePrediction::Goal>
  {
    size_t operator()(const RoutePrediction::Goal & x) const
    {
      return x.get_hash();
    }
  };
}
