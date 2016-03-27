/*
 * CityObj.h
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#ifndef CITY_CITY_H_
#define CITY_CITY_H_
#define  max_slope_percent 6

#include <string>
#include <map>

#include "Intersection.h"
#include "Road.h"
#include "../driver_prediction/Link.h"

using namespace DriverPrediction;

namespace City {

class CityObj {
public:
	Intersection* intersections;
	int dateTimeCreated;
	std::string roadFileName;
	std:: string intersectionFileName;
	int intervalDistance = 1;
	std::map<int,int> intersectionNumMap;
	/**
	%maps the number of a intersection to the index in intersections
        %if an index in intersection_num_map is 0, then there is no
        %intersection with that number, no two index's should be able to to
        %map to the same index, each intersection needs to be reprsented in
        %the map exactly once
	**/
	Road* road_list; //list of roads in city should be indexed by road unique number
	CityObj(Intersection*, std::string, std::string, int, std::map<int, int>);
	CityObj();
	int getRoadListSize();
	int getInstersectionListSize();
	Link* getNextLinks(Link* link);
	Intersection* getIntersectionFromLink(Link* link);
	virtual ~CityObj();
};

} /* namespace City */

#endif /* CITY_CITY_H_ */
