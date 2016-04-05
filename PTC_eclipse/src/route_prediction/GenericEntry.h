/*
 * GenericEntry.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICENTRY_H_
#define ROUTE_PREDICTION_GENERICENTRY_H_

namespace PredictivePowertrain {

template<class K, class V>
class GenericEntry {
public:
	K key;
	V value;
	GenericEntry(K, V);
	virtual ~GenericEntry();
};


template<class K, class V>
GenericEntry<K,V>::GenericEntry(K key, V value) {
	// TODO Auto-generated constructor stub
	this->key = key;
	this->value = value;

}

template<class K, class V>
GenericEntry<K,V>::~GenericEntry() {
	// TODO Auto-generated destructor stub
}

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GENERICENTRY_H_ */
