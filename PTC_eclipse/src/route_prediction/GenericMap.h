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

template<class K, class V>
InnovationModel::GenericMap<K, V>::GenericMap() {
	this->map = std::map<K,V>();
	this->iter = NULL;
	this->hash_counter = 0;
	this->array_counter = 0;
}

template<class K, class V>
InnovationModel::GenericMap<K, V>::GenericMap(GenericMap& other) {
	this->map = std::map<K,V>();
	this->iter = NULL;
	this->hash_counter = 0;
	this->array_counter = 0;
}

template<class K, class V>
void InnovationModel::GenericMap<K, V>::initialize_counter() {
	this->hash_counter = 0;
	this->array_counter = 0;
}

template<class K, class V>
GenericEntry<K, V> InnovationModel::GenericMap<K, V>::next_entry() {
	if (this->iter == NULL) {
		this->iter = this->map.begin();
	}
	if (this->iter != this->map.end()) {
		GenericEntry<K,V> entry = GenericEntry<K,V>(iter->first, iter->second);
		return entry;
		this->iter++;
	} else {
		return NULL;
	}
}

template<class K, class V>
GenericEntry<K, V> InnovationModel::GenericMap<K, V>::get_min_entry() {
	typename std::map<K,V>::iterator iter = this->map.begin();
	V min = NULL;
	if (iter != this->map.end()) {
		min = iter.second;
		iter++;
		while (iter != this->map.end()) {
			if (min > iter.second) {
				min = iter.second;
			}
		}
	}
	return min;
}

template<class K, class V>
bool InnovationModel::GenericMap<K, V>::hash_in_map(K key) {
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return true;
	} else {
		return false;
	}
}

template<class K, class V>
V InnovationModel::GenericMap<K, V>::get_entry(K key) {
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return iter.second;
	} else {
		return NULL;
	}
}

template<class K, class V>
typename std::map<K, V>::iterator InnovationModel::GenericMap<K, V>::begin() {
	return map.begin();
}

template<class K, class V>
typename std::map<K, V>::iterator InnovationModel::GenericMap<K, V>::end() {
	return map.end();
}

template<class K, class V>
int InnovationModel::GenericMap<K, V>::get_size() {
	return this->map.size();
}

template<class K, class V>
int InnovationModel::GenericMap<K, V>::add_entry(K key, V value) {
	if (this->map.insert (std::pair<K,V>(key, value)).second == true) {
		return 1;
	} else {
		return 0;
	}
}

template<class K, class V>
GenericMap<K,V>::~GenericMap() {
	// TODO Auto-generated destructor stub
}


} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_GENERICMAP_H_ */
