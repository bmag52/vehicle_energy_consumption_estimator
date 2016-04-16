/*
 * Way.cpp
 *
 *  Created on: Apr 15, 2016
 *      Author: vagrant
 */

#include "Way.h"

namespace PredictivePowertrain {

Way::Way() {
}

Way::Way(GenericMap<int, long int>* nodeIDs, long int id) {
	this->NodeIDs = nodeIDs;
	this->id = id;
}

GenericMap<int, long int>* Way::getNodeIDs() {
	return this->NodeIDs;
}

long int Way::getID() {
	return this->id;
}

}
