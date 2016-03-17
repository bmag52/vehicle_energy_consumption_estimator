/*
 * GenericEntry.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#include "GenericEntry.h"

namespace InnovationModel {

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

} /* namespace InnovationModel */
