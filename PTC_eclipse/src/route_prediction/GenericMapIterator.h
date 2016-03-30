/*
 * GenericMapIterator.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICMAPITERATOR_H_
#define ROUTE_PREDICTION_GENERICMAPITERATOR_H_
#include "GenericMap.h"

namespace PredictivePowertrain {

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


template<class K, class V>
RoutePrediction::GenericMapIterator<K, V>::GenericMapIterator(
		GenericMap<K, V> map) {
	this->map = map;
	this->iterator = map.begin();
}

template<class K, class V>
GenericEntry<K, V> RoutePrediction::GenericMapIterator<K, V>::next_entry() {
	if (iterator != map.end()) {
		GenericEntry<K,V> entry = GenericEntry<K,V>(iterator->first, iterator->second);
		++iterator;
		return entry;
	} else {
		return NULL;
	}
}

template<class K, class V>
GenericMapIterator<K,V>::~GenericMapIterator() {
}


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GENERICMAPITERATOR_H_ */
