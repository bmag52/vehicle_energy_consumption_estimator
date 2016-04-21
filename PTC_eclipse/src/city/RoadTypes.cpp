/*
 * RoadTypes.cpp
 *
 *  Created on: Jan 7, 2016
 *      Author: Amanda
 */

#include "RoadTypes.h"
#include <string>
#include <limits>

using namespace std;

namespace PredictivePowertrain {

//roadType can either be default, city, country, or interstate
//assume user inputs string correctly for four options

RoadTypes::RoadTypes(string color, int min_speed, int max_speed, int min_distance, int max_distance, string string) {
	this->color = color;
	this->min_speed = min_speed;
	this->max_speed = max_speed;
	this->min_distance = min_distance;
	this->max_distance = max_distance;
	this->toString = string;
}


RoadTypes::RoadTypes(string roadType) {
	if (roadType == "default") {
		color = "yellow";
		min_speed = 25;
		max_speed = 130;
		min_distance = 0;
		max_distance = numeric_limits<int>::max();
		toString = "Road Type: " + roadType + " Color: " + color + " Min Speed: " + to_string(min_speed) + " Max Speed: "
				+ to_string(max_speed) + " Min Distance " + to_string(min_distance) + " Max Distance" + to_string(max_distance);
	} else if (roadType == "city") {
		color = "red";
		min_speed = 25;
		max_speed = 55;
		min_distance = 500;
		max_distance = 1000;
		toString = "Road Type: " + roadType + " Color: " + color + " Min Speed: " + to_string(min_speed) + " Max Speed: "
				+ to_string(max_speed) + " Min Distance " + to_string(min_distance) + " Max Distance" + to_string(max_distance);
	} else if (roadType == "country") {
		color = "blue";
		min_speed = 40;
		max_speed = 90;
		min_distance = 1000;
		max_distance = 2000;
		toString = "Road Type: " + roadType + " Color: " + color + " Min Speed: " + to_string(min_speed) + " Max Speed: "
				+ to_string(max_speed) + " Min Distance " + to_string(min_distance) + " Max Distance" + to_string(max_distance);
	} else if (roadType == "interstate") {
		color = "black";
		min_speed = 95;
		max_speed = 130;
		min_distance = 1000;
		max_distance = numeric_limits<int>::max();
		toString = "Road Type: " + roadType + " Color: " + color + " Min Speed: " + to_string(min_speed) + " Max Speed: "
				+ to_string(max_speed) + " Min Distance " + to_string(min_distance) + " Max Distance" + to_string(max_distance);
	}
}

RoadTypes::~RoadTypes() {

}

} /* namespace PredictivePowertrain */
