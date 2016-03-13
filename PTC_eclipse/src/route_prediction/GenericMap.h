/*
 * GenericMap.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICMAP_H_
#define ROUTE_PREDICTION_GENERICMAP_H_
#include <functional>
#include <map>
#include "GenericEntry.h"

namespace InnovationModel {

template<class K, class V>
class GenericMap {
private:
	std::map<K,V> map;
	int hash_counter;
	int array_counter;
	int size;
public:
	GenericMap();
	GenericMap(GenericMap& other);
	void initialize_counter();
	GenericEntry<K,V> next_entry();
	GenericEntry<K,V> get_min_entry();
	bool hash_in_map(K key);
	bool get_entry(K key);
	int get_size();
	int sum_function_call(const std::function<int(int)> &f);
	int add_entry(K key, V value);
	virtual ~GenericMap();
};

} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_GENERICMAP_H_ */
