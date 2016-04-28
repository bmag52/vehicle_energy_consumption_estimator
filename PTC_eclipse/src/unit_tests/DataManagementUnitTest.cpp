/*
 * DataManagement_ut.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: vagrant
 */


#include "UnitTests.h"
#include "../data_management/DataManagement.h"

using namespace PredictivePowertrain;

void dataManagement_ut() {

	DataManagement testDM;

	// test trip log add and get
	GenericMap<double, double>* latLon = new GenericMap<double, double>();
	latLon->addEntry(47.654, -122.345);
	latLon->addEntry(47.653, -122.346);
	latLon->addEntry(47.652, -122.347);
	testDM.addTripData(latLon);
	testDM.addTripData(latLon);
	GenericMap<double, double>* storedLatLon = testDM.getMostRecentTripData();
	assert(storedLatLon->getSize() == latLon->getSize());

	storedLatLon->initializeCounter();
	bool latLonsMatch = true;
	GenericEntry<double, double>* nextStoredLatLon = storedLatLon->nextEntry();
	while(nextStoredLatLon != NULL)
	{
		if(!latLon->hasEntry(nextStoredLatLon->key) || nextStoredLatLon->value != latLon->getEntry(nextStoredLatLon->key))
		{
			latLonsMatch = false;
		}
		nextStoredLatLon = storedLatLon->nextEntry();
	}
	assert(latLonsMatch);

	// test city logging add
	GenericMap<int, Node*>* nodes = new GenericMap<int, Node*>();
	nodes->addEntry(1, new Node(42.3, -122.4, 44, 1234566));
	nodes->addEntry(2, new Node(42.3, -122.4, 44, 1234566));

	GenericMap<long int, Road*>* blankRoads;
	GenericMap<int, Bounds*>* boundsMap = new GenericMap<int, Bounds*>();
	boundsMap->addEntry(0, new Bounds(2, 3, 4, 5));
	boundsMap->addEntry(1, new Bounds(2, 3, 4, 5));
	boundsMap->addEntry(2, new Bounds(2, 3, 4, 5));

	Intersection* startInt = new Intersection(blankRoads, 42.3, -122.4, 44, 345);
	Intersection* endInt = new Intersection(blankRoads, 42.3, -122.4, 44, 456);

	Road* road1 = new Road("hilly", 987654, nodes);
	road1->setStartIntersection(startInt);
	road1->setEndIntersection(endInt);
	road1->setBoundsID(0);

	Road* road2 = new Road("flat", 8765, nodes);
	road2->setStartIntersection(startInt);
	road2->setEndIntersection(endInt);
	road2->setBoundsID(1);

	Road* road3 = new Road("medium", 9854, nodes);
	road3->setStartIntersection(startInt);
	road3->setEndIntersection(endInt);
	road3->setBoundsID(2);

	GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
	roads->addEntry(road1->getRoadID(), road1);
	roads->addEntry(road2->getRoadID(), road2);
	roads->addEntry(road3->getRoadID(), road3);

	Intersection* intersection1 = new Intersection(roads, 42.3, -122.4, 44, 123);
	intersection1->setBoundsID(0);
	Intersection* intersection2 = new Intersection(roads, 42.3, -122.4, 44, 234);
	intersection2->setBoundsID(0);
	Intersection* intersection3 = new Intersection(roads, 42.3, -122.4, 44, 345);
	intersection3->setBoundsID(1);
	Intersection* intersection4 = new Intersection(roads, 42.3, -122.4, 44, 456);
	intersection4->setBoundsID(2);

	GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
	intersections->addEntry(intersection1->getIntersectionID(), intersection1);
	intersections->addEntry(intersection2->getIntersectionID(), intersection2);
	intersections->addEntry(intersection3->getIntersectionID(), intersection3);
	intersections->addEntry(intersection4->getIntersectionID(), intersection4);

	City* city = new City(intersections, roads, boundsMap);
	testDM.addCityData(city);

	City* storedCity = testDM.getCityData();
	assert(city->getInstersectionMapSize() == storedCity->getInstersectionMapSize());
	assert(city->getRoadMapSize() == storedCity->getRoadMapSize());
	assert(city->getBoundsMapSize() == storedCity->getBoundsMapSize());

	GenericMap<long int, Road*>* storedRoads = storedCity->getRoads();
	bool hasAllRoads = true;
	storedRoads->initializeCounter();
	GenericEntry<long int, Road*>* nextStoredRoad = storedRoads->nextEntry();
	while(nextStoredRoad != NULL)
	{
		if(!roads->hasEntry(nextStoredRoad->key))
		{
			hasAllRoads = false;
		}
		assert(intersections->hasEntry(nextStoredRoad->value->getStartIntersection()->getIntersectionID()));
		assert(intersections->hasEntry(nextStoredRoad->value->getEndIntersection()->getIntersectionID()));
		nextStoredRoad = storedRoads->nextEntry();
	}
	assert(hasAllRoads);

	GenericMap<int, Intersection*>* storedInts = storedCity->getIntersections();
	bool hasAllInts = true;
	storedInts->initializeCounter();
	GenericEntry<int, Intersection*>* nextStoredInt = storedInts->nextEntry();
	while(nextStoredInt != NULL)
	{
		if(!intersections->hasEntry(nextStoredInt->key))
		{
			hasAllInts = false;
		}
		GenericMap<long int, Road*>* connectingRoads = nextStoredInt->value->getRoads();
		bool hasAllConnectingRoads = true;
		connectingRoads->initializeCounter();
		GenericEntry<long int, Road*>* nextStoredConnectingRoad = connectingRoads->nextEntry();
		while(nextStoredConnectingRoad != NULL)
		{
			if(!roads->hasEntry(nextStoredConnectingRoad->key))
			{
				hasAllConnectingRoads = false;
			}
			nextStoredConnectingRoad = connectingRoads->nextEntry();
		}
		nextStoredInt = storedInts->nextEntry();
		assert(hasAllConnectingRoads);
	}
	assert(hasAllInts);

	GenericMap<int, Bounds*>* storedBounds = storedCity->getBoundsMap();
	bool hasAllBounds = true;
	storedBounds->initializeCounter();
	GenericEntry<int, Bounds*>* nextStoredBounds = storedBounds->nextEntry();
	while(nextStoredBounds != NULL)
	{
		if(!boundsMap->hasEntry(nextStoredBounds->key))
		{
			hasAllBounds = false;
		}
		nextStoredBounds = storedBounds->nextEntry();
	}
	assert(hasAllBounds);


}

