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

namespace RoutePrediction {

template<typename K, typename V>
class GenericMap {
private:
	std::map<K,V> map;
	typename std::map<K,V>::iterator iter;
	int hashCounter; // might not be needed, use iterator
	int arrayCounter; // might not be needed, use iterator
public:
	GenericMap();
	GenericMap(GenericMap& other);
	GenericMap* copy();
	void initializeCounter();
	GenericEntry<K,V>* nextEntry();
	GenericEntry<K,V>* getMinEntry();
	bool hashInMap(K key);
	V getEntry(K key);
	typename std::map<K,V>::iterator iterator();
	typename std::map<K,V>::iterator begin();
	typename std::map<K,V>::iterator end();
	int getSize();
	int addEntry(K key, V value);
	virtual ~GenericMap();
};

template<class K, class V>
GenericMap<K, V>::GenericMap() {
	this->map = std::map<K,V>();
	this->iter = map.end();
	this->hashCounter = 0;
	this->arrayCounter = 0;
}

template<class K, class V>
GenericMap<K, V>::GenericMap(GenericMap& other) {
	this->map = std::map<K,V>();
	this->iter = map.end();
	this->hashCounter = 0;
	this->arrayCounter = 0;
}

template<class K, class V>
GenericMap<K, V>* GenericMap<K, V>::copy() {
	// TODO copy();
}

template<class K, class V>
void GenericMap<K, V>::initializeCounter() {
	this->hashCounter = 0;
	this->arrayCounter = 0;
}

template<class K, class V>
GenericEntry<K,V>* GenericMap<K, V>::nextEntry() {
	if (this->iter == this->map.end()) {
		this->iter = this->map.begin();
	}
	GenericEntry<K,V> entry = GenericEntry<K,V>(this->iter->first,this->iter->second);
	this->iter++;
	return &entry;
}

template<class K, class V>
GenericEntry<K, V>* GenericMap<K, V>::getMinEntry() {
	typename std::map<K,V>::iterator iter = this->map.begin();
	V min = NULL;
	if (iter != this->map.end()) {
		min = iter->second;
		iter++;
		while (iter != this->map.end()) {
			if (min > iter->second) {
				min = iter->second;
			}
		}
	}
	return &min;
}

template<class K, class V>
bool GenericMap<K, V>::hashInMap(K key) {
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return true;
	} else {
		return false;
	}
}

template<class K, class V>
V GenericMap<K, V>::getEntry(K key) {
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return iter->second;
	} else {
		return NULL;
	}
}

template<class K, class V>
typename std::map<K, V>::iterator GenericMap<K, V>::begin() {
	return map.begin();
}

template<class K, class V>
typename std::map<K, V>::iterator GenericMap<K, V>::end() {
	return map.end();
}

template<class K, class V>
int RoutePrediction::GenericMap<K, V>::getSize() {
	return this->map.size();
}

template<class K, class V>
int GenericMap<K, V>::addEntry(K key, V value) {
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


} /* namespace RoutePrediction */

#endif /* ROUTE_PREDICTION_GENERICMAP_H_ */
