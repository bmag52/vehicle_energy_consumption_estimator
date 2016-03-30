/*
 * Road.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROAD_H_
#define ROAD_H_

#include <string>

namespace PredictivePowertrain {

class Road {
private:
	int* elevationData;
	int* speedLimitData;
	int startNumber;
	int endNumber;
//	RoadTypes roadType;
	int roadID;

public:
	Road();
	Road(std::string roadType, int* elevationData, int* speedData, int roadID);
	virtual ~Road();
	void setStartNumber(int intersectionNumber);
	int getStartNumber();
	void setEndNumber(int intersectionNumber);
	int getEndNumber();
	int getRoadID();
};

} /* namespace PredictivePowertrain */

#endif /* ROAD_H_ */
