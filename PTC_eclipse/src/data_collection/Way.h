/*
 * Way.h
 *
 *  Created on: Apr 15, 2016
 *      Author: vagrant
 */

#ifndef DATA_COLLECTION_WAY_H_
#define DATA_COLLECTION_WAY_H_

#include "../map/GenericMap.h"
#include "Node.h"
#include <string>

namespace PredictivePowertrain {

class Way {
private:
	GenericMap<int, long int>* nodeIDs;
	std::string wayType;
	int waySpeed;
	long int id;

public:
	Way();
	Way(GenericMap<int, long int>* nodeIDs, long int id, std::string wayType, int waySpeed);
	GenericMap<int, long int>* getNodeIDs();
	long int getID();
	std::string getWayType();
	int getWaySpeed();
};

}

#endif /* DATA_COLLECTION_WAY_H_ */
