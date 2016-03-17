/*
 * GenericMap.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "GenericMap.h"
#include "stddef.h"
#include <map>

namespace InnovationModel {

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
