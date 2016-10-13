/*
 * Way.h
 *
 *  Created on: Apr 15, 2016
 *      Author: vagrant
 */

#ifndef DATA_MANAGEMENT_WAY_H_
#define DATA_MANAGEMENT_WAY_H_

#include "../map/GenericMap.h"
#include <string>
#include "../data_management/Node.h"

namespace PredictivePowertrain {

class Way {
private:
	GenericMap<int, long int>* nodeIDs;
	std::string wayType;
	int waySpeed;
	long int ID;

public:
	Way();
	Way(GenericMap<int, long int>* nodeIDs, long int id, std::string wayType, int waySpeed);
	GenericMap<int, long int>* getNodeIDs();
	long int getID();
	std::string getWayType();
	int getWaySpeed();
};

}

#endif /* DATA_MANAGEMENT_WAY_H_ */
