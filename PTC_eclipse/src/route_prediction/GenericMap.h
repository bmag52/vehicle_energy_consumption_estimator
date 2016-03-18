/*
 * GenericMap.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICMAP_H_
#define ROUTE_PREDICTION_GENERICMAP_H_
#include "stddef.h"
#include <functional>
#include <map>
#include "GenericEntry.h"

namespace InnovationModel {

template<typename K, typename V>
class GenericMap {
private:
	std::map<K,V> map;
	typename std::map<K,V>::iterator iter;
	int hash_counter; // might not be needed, use iterator
	int array_counter; // might not be needed, use iterator
public:
	GenericMap();
	GenericMap(GenericMap& other);
	void initialize_counter();
	GenericEntry<K,V> next_entry();
	GenericEntry<K,V> get_min_entry();
	bool hash_in_map(K key);
	V get_entry(K key);
	typename std::map<K,V>::iterator iterator();
	typename std::map<K,V>::iterator begin();
	typename std::map<K,V>::iterator end();
	int get_size();
	int add_entry(K key, V value);
	virtual ~GenericMap();
};

} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_GENERICMAP_H_ */
