/*
 * GenericMapIterator.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICMAPITERATOR_H_
#define ROUTE_PREDICTION_GENERICMAPITERATOR_H_
#include "GenericMap.h"

namespace InnovationModel {

template<class K, class V>
class GenericMapIterator {
private:
	GenericMap<K,V> map;
	typename std::map<K,V>::iterator iterator;
public:
	GenericMapIterator(GenericMap<K,V> map);
	GenericEntry<K,V> next_entry();
	virtual ~GenericMapIterator();
};

} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_GENERICMAPITERATOR_H_ */
