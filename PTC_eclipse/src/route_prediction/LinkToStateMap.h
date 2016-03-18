/*
 * LinkToStateMap.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_LINKTOSTATEMAP_H_
#define ROUTE_PREDICTION_LINKTOSTATEMAP_H_
#include "GenericMap.h"
#include "LinkStateMapEntry.h"
#include "Goal.h"

namespace InnovationModel {

class LinkToStateMap {
public:
	GenericMap<int, LinkStateMapEntry> maps;
	int increment_transition(Link, Goal, Link);
	GenericEntry<double, double> get_probability(Link, Link, Goal, bool);
	LinkToStateMap();
	LinkToStateMap(LinkToStateMap &other);
	virtual ~LinkToStateMap();
};

} /* namespace InnovationModel */

#endif /* ROUTE_PREDICTION_LINKTOSTATEMAP_H_ */
