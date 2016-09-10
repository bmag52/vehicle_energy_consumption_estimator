/*
 * GoalMapEntry.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALMAPENTRY_H_
#define ROUTE_PREDICTION_GOALMAPENTRY_H_
#include "../map/GenericMap.h"
#include "LinkToStateMapEntry.h"
#include "Goal.h"

namespace PredictivePowertrain {

template<typename K, typename V>
class GoalMapEntry {
private:
	GenericMap<K, V>* map;
	Goal* goal;
	int m;
    void initialize();

public:
	GoalMapEntry();
	GoalMapEntry(Goal* goal);
	void incrementCount();
	virtual ~GoalMapEntry();
	Goal* getGoal();
	V getMapEntry(K key);
    void addMapEntry(K key, V val);
	int getM();
    GenericMap<K,V>* getMap();
};
    
template<class K, class V>
GoalMapEntry<K, V>::GoalMapEntry()
{
    this->goal = new Goal();
    this->initialize();
}

template<class K, class V>
GoalMapEntry<K, V>::GoalMapEntry(Goal* goal)
{
    this->goal = goal;
    this->initialize();
}

template<class K, class V>
void GoalMapEntry<K, V>::initialize()
{
    this->m = 0;
    this->map = new GenericMap<K, V>();
}

template<class K, class V>
GoalMapEntry<K, V>::~GoalMapEntry()
{
    free(this->goal);
    free(this->map);
}

template<class K, class V>
void GoalMapEntry<K, V>::addMapEntry(K key, V value)
{
    this->map->addEntry(key, value);
}

template<class K, class V>
void GoalMapEntry<K, V>::incrementCount()
{
    this->m++;
}

template<class K, class V>
Goal* GoalMapEntry<K, V>::getGoal()
{
    return this->goal;
}

template<class K, class V>
V GoalMapEntry<K, V>::getMapEntry(K key)
{
    return this->map->getEntry(key);
}

template<class K, class V>
int GoalMapEntry<K, V>::getM()
{
    return this->m;
}

template<class K, class V>
GenericMap<K, V>* GoalMapEntry<K, V>::getMap()
{
    return this->map;
}


} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GOALMAPENTRY_H_ */
