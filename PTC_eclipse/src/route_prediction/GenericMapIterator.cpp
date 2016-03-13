/*
 * GenericMapIterator.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "GenericMapIterator.h"

namespace InnovationModel {

template<class K, class V>
InnovationModel::GenericMapIterator<K, V>::GenericMapIterator(
		GenericMap<K, V> map) {
	this->map = map;
	this->iterator = map.begin();
}

template<class K, class V>
GenericEntry<K, V> InnovationModel::GenericMapIterator<K, V>::next_entry() {
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
	// TODO Auto-generated destructor stub
}

} /* namespace InnovationModel */
