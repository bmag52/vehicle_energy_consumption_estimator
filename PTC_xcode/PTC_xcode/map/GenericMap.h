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
#include <iostream>
#include <map>
#include "GenericEntry.h"

namespace PredictivePowertrain {

template<typename K, typename V>
class GenericMap {
private:
	std::map<K,V> map;
	typename std::map<K,V>::iterator iter;
	bool hasNextEntry;
public:
	GenericMap();
	GenericMap(GenericMap& other);
	GenericMap* copy();
	void initializeCounter();
	GenericEntry<K,V>* nextEntry();
	GenericEntry<K,V>* getMinEntry();
	bool hasEntry(K key);
	V getEntry(K key);
    V getFirstEntry();
    V getLastEntry();
	typename std::map<K,V>::iterator iterator();
	typename std::map<K,V>::iterator begin();
	typename std::map<K,V>::iterator end();
	int getSize();
	int addEntry(K key, V value);
	void updateEntry(K key, V value);
	virtual ~GenericMap();
	bool erase(K key);
    bool indexErase(K key);
};

template<class K, class V>
GenericMap<K, V>::GenericMap()
{
	this->map = std::map<K,V>();
	this->iter = map.end();
	this->hasNextEntry = false;
}

template<class K, class V>
GenericMap<K, V>::GenericMap(GenericMap& other)
{
	this->map = std::map<K,V>();
	this->iter = map.end();
	this->hashCounter = 0;
	this->arrayCounter = 0;
	this->hasNextEntry = false;
}

template<class K, class V>
GenericMap<K, V>* GenericMap<K, V>::copy()
{
	GenericMap<K, V>* newMap = new GenericMap<K, V>();

	this->initializeCounter();
	GenericEntry<K,V>* nextEntry = this->nextEntry();
	while(nextEntry != NULL)
	{
		newMap->addEntry(nextEntry->key, nextEntry->value);
		nextEntry = this->nextEntry();
	}
	delete(nextEntry);
	return newMap;
}

template<class K, class V>
void GenericMap<K, V>::initializeCounter()
{
	if(this->getSize() > 0)
	{
		this->iter = this->map.begin();
		this->hasNextEntry = true;
	}
}

template<class K, class V>
GenericEntry<K,V>* GenericMap<K, V>::nextEntry()
{
	GenericEntry<K,V>* entry = NULL;
	if(this->hasNextEntry) {
		entry = new GenericEntry<K,V>(this->iter->first,this->iter->second);
		this->iter++;
	}

	if(this->iter == this->map.end()) {
		this->hasNextEntry = false;
	}
	return entry;
}

template<class K, class V>
GenericEntry<K, V>* GenericMap<K, V>::getMinEntry()
{
	typename std::map<K,V>::iterator iter = this->map.begin();
	V minVal = NULL;
	K key = NULL;
	if (iter != this->map.end()) {
		minVal = iter->second;
		key = iter->first;
		iter++;
		while (iter != this->map.end()) {
			if (minVal > iter->second) {
				minVal = iter->second;
				key = iter->first;
			}
			iter++;
		}
	}
	GenericEntry<K,V>* entry = new GenericEntry<K,V>(key, minVal);
	return entry;
}

template<class K, class V>
bool GenericMap<K, V>::hasEntry(K key)
{
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return true;
	} else {
		return false;
	}
}

template<class K, class V>
bool GenericMap<K, V>::erase(K key)
{
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		this->map.erase(iter);
        
		return true;
	} else {
		return false;
	}
}
    
template<class K, class V>
bool GenericMap<K, V>::indexErase(K key)
{
    typename std::map<K,V>::iterator iter = this->map.find(key);
    if (iter != this->map.end()) {
        
        iter = this->map.erase(iter);
        
        while(iter != this->map.end())
        {
            V entry = this->getEntry(iter->first);
            this->addEntry(iter->first - 1, entry);
            iter = this->map.erase(iter);
        }
        
        if(this->hasNextEntry)
        {
            K searchKey = key;
            if(searchKey != 0)
            {
                searchKey -= 1;
            }
            
            this->iter = this->map.find(searchKey);
        }
        
        return true;
    } else {
        return false;
    }
    
}

template<class K, class V>
V GenericMap<K, V>::getEntry(K key)
{
	typename std::map<K,V>::iterator iter = this->map.find(key);
	if (iter != this->map.end()) {
		return iter->second;
	} else {
		return NULL;
	}
}
    
template<class K, class V>
V GenericMap<K, V>::getFirstEntry()
{
    
    if (this->getSize() > 0) {
        return this->begin()->second;
    } else {
        return NULL;
    }
}
    
template<class K, class V>
V GenericMap<K, V>::getLastEntry()
{
    
    if (this->getSize() > 0) {
        return this->map.rbegin()->second;
    } else {
        return NULL;
    }
}
    
template<class K, class V>
typename std::map<K, V>::iterator GenericMap<K, V>::begin()
{
	return this->map.begin();
}

template<class K, class V>
typename std::map<K, V>::iterator GenericMap<K, V>::end()
{
	return this->map.end();
}

template<class K, class V>
int GenericMap<K, V>::getSize()
{
	return this->map.size();
}

template<class K, class V>
int GenericMap<K, V>::addEntry(K key, V value)
{
	if(this->getEntry(key) != NULL) {
		return 0;
	} else if(this->map.insert(std::pair<K,V>(key, value)).second == true) {
		return 1;
	} else {
		return 0;
	}
}

template<class K, class V>
void GenericMap<K, V>::updateEntry(K key, V value)
{
	typename std::map<K,V>::iterator iter = this->map.find(key);
		if (iter != this->map.end())
        {
			this->map.erase(iter);
		}
		this->addEntry(key, value);
}

template<class K, class V>
GenericMap<K,V>::~GenericMap()
{
    typename std::map<K,V>::iterator iter = this->map.begin();
    
    while(iter != this->map.end())
    {
        iter = this->map.erase(iter);
    }
}


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GENERICMAP_H_ */
