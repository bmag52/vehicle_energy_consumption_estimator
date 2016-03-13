/*
 * GenericMap.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "GenericMap.h"

namespace InnovationModel {

template<class K, class V>
GenericMap<K,V>::GenericMap() {
	// TODO Auto-generated constructor stub

}

template<class K, class V>
inline GenericMap<K,V>::GenericMap(GenericMap& other)
{
}

template<class K, class V>
inline void initialize_counter() {
}

template<class K, class V>
inline GenericEntry<K,V> next_entry() {
	return NULL;
}

template<class K, class V>
inline GenericEntry<K,V> get_min_entry() {
	return NULL;
}

template<class K, class V>
inline bool hash_in_map(K key) {
}

template<class K, class V>
inline bool get_entry(K key) {
}

template<class K, class V>
inline int get_size() {
}

template<class K, class V>
inline int sum_function_call(const std::function<int(int)>& f) {
}

template<class K, class V>
inline int add_entry(K key, V value) {
}

template<class K, class V>
GenericMap<K,V>::~GenericMap() {
	// TODO Auto-generated destructor stub
}

} /* namespace InnovationModel */
