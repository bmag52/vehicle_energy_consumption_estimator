/*
 * GenericMap.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "GenericMap.h"

namespace InnovationModel {

template<class K, class V>
InnovationModel::GenericMap<K, V>::GenericMap() {
}

template<class K, class V>
InnovationModel::GenericMap<K, V>::GenericMap(GenericMap& other) {
}

template<class K, class V>
void InnovationModel::GenericMap<K, V>::initialize_counter() {
}

template<class K, class V>
GenericEntry<K, V> InnovationModel::GenericMap<K, V>::next_entry() {
}

template<class K, class V>
GenericEntry<K, V> InnovationModel::GenericMap<K, V>::get_min_entry() {
}

template<class K, class V>
bool InnovationModel::GenericMap<K, V>::hash_in_map(K key) {
}

template<class K, class V>
bool InnovationModel::GenericMap<K, V>::get_entry(K key) {
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
}

template<class K, class V>
int InnovationModel::GenericMap<K, V>::sum_function_call(
		const std::function<int(int)>& f) {
}

template<class K, class V>
int InnovationModel::GenericMap<K, V>::add_entry(K key, V value) {
}

template<class K, class V>
GenericMap<K,V>::~GenericMap() {
	// TODO Auto-generated destructor stub
}

} /* namespace InnovationModel */
