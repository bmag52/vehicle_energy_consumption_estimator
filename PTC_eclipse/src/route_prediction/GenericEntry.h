/*
 * GenericEntry.h
 *
 *  Created on: Mar 12, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GENERICENTRY_H_
#define ROUTE_PREDICTION_GENERICENTRY_H_

namespace InnovationModel {

template<class K, class V>
class GenericEntry {
public:
	K key;
	V value;
	GenericEntry(K, V);
	virtual ~GenericEntry();
};

} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_GENERICENTRY_H_ */
