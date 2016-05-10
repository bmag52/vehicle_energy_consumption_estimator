/*
 * LinkStateMapEntry.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_LINKTOSTATEMAPENTRY_H_
#define ROUTE_PREDICTION_LINKTOSTATEMAPENTRY_H_
#include "../map/GenericMap.h"
#include "../driver_prediction/Link.h"



namespace PredictivePowertrain {

class LinkToStateMapEntry {
private:
	GenericMap<int,int> entries;
public:
	LinkToStateMapEntry();
	LinkToStateMapEntry(LinkToStateMapEntry &other);
	int addEntry(Link* li);
	int getM(Link* li);
	int getTotalM();
	virtual ~LinkToStateMapEntry();
	static int totalM(LinkToStateMapEntry* linkToStateMapEntry);
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_LINKTOSTATEMAPENTRY_H_ */
