/*
 * IntersectionUnitTest.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: vagrant
 */


#include "../city/Intersection.h"
#include "../city/Road.h"
#include "../city/RoadTypes.h"
#include "../driver_prediction/Link.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"

using namespace std;

void intersection_ut(){
	//Road(std::string roadType, int* elevationData, int* speedData, int roadID, double* lat, double* lon)
	Road road1;
	Road road2;


	//Intersection(Road * roadInput, double lat, double lon, int elev, int intersectNum)
	Intersection intersection1(&road1, 0, 0, 0, 0);
	Intersection intersection2(&road1, 1.0, 1.0, 1, 1);
	Intersection intersection3(&road2, 10.0, 10.0, 4, 5);
	Intersection intersection4;
	Intersection intersection5;

	//Testing getNumber(), getElevation(), getLat(), and getLon()
	assert(intersection1.getNumber() == 0 && intersection1.getElevation() == 0);
	assert(intersection1.getLat() == 0 && intersection1.getLon() == 0);

	assert(intersection2.getNumber() == 1 && intersection2.getElevation() == 1);
	assert(intersection2.getLat() == 1.0 && intersection2.getLon() == 1.0);


	//Testing getRoads()
	assert(intersection2.getRoads() == &road1);
	assert(intersection3.getRoads() == &road2);

	//Testing addRoad() and getRoadCount()
	intersection5.addRoad(&road2, 0);
	assert(intersection5.getRoadCount() == 1);

	//Testing getOutgoingLinks()
	Link link1;
	assert(intersection4.getOutgoingLinks() == &link1);


	//Testing getAdjacentIntersection()
	//assert(intersection4.getAdjacentIntersection() == NULL);

	//Testing getNextIntersection()
	assert(intersection4.getNextIntersection(&road1) == NULL);


}
