/*
 * Goal.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: vagrant
 */

#include "Goal.h"

namespace InnovationModel {

Goal::Goal(int destination, int bin[], int size) {
	this->destination = destination;
	this->bins = bin;
	this->size = size;
}

Goal::Goal(Goal& other) {
	this->destination = other.destination;
	this->size = other.size;
	this->bins = new int[size];
	for (size_t i = 0; i < size; i++) {
		this->bins[i] = other.bins[i];
	}
}

bool Goal::issimilar(Goal& other) {
	if (this->size == other.size) {
		for (size_t i = 0; i < size; i++) {
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

Goal::~Goal() {
	// TODO Auto-generated destructor stub
}

} /* namespace InnovationModel */
