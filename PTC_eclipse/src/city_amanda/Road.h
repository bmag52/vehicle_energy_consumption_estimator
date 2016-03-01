/*
 * Road.h
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#ifndef ROAD_H_
#define ROAD_H_

namespace City {

class Road {
public:
	int* elevation_data;
	int* speed_limit_data;
	int start_node;
	int end_node;
//	RoadTypes road_type;
	int road_id;
//	Road(std::string, int*, int*, int);
	virtual ~Road();
};

} /* namespace City */

#endif /* ROAD_H_ */
