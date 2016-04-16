/*
 * Way.h
 *
 *  Created on: Apr 15, 2016
 *      Author: vagrant
 */

#ifndef DATA_COLLECTION_WAY_H_
#define DATA_COLLECTION_WAY_H_

#include "../map/GenericMap.h"

namespace PredictivePowertrain {

class Way {
private:
	GenericMap<int, long int>* NodeIDs;
	long int id;

public:
	Way();
	Way(GenericMap<int, long int>* nodeIDs, long int id);
	GenericMap<int, long int>* getNodeIDs();
	long int getID();
};

}

#endif /* DATA_COLLECTION_WAY_H_ */
