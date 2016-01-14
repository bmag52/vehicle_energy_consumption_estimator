/*
 * Road.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "Road.h"

namespace City {


Road::Road(std::string roadType, int* elevationData, int* speedData, int roadID) {
	elevation_data = elevationData;
	speed_limit_data = speedData;
	road_type = new RoadTypes(roadType);
	road_id = roadID;
	start_node = 0;
	end_node = 0;

}

Road::~Road() {
	delete elevation_data;
	delete speed_limit_data;
}

} /* namespace City */
