/*
 * RoadTypes.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROADTYPES_H_
#define ROADTYPES_H_
#include <string>

using namespace std;

namespace PredictivePowertrain {

class RoadTypes {
public:
	string color;
	int max_speed;
	int min_speed;
	int max_distance;
	int min_distance;
	string toString;
	RoadTypes(string);
	RoadTypes(string, int, int, int, int, string);
	virtual ~RoadTypes();
};

} /* namespace PredictivePowertrain */

#endif /* ROADTYPES_H_ */
