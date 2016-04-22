/*
 * DataManagement.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#include "DataManagement.h"

using boost::property_tree::ptree;
using boost::lexical_cast;

namespace PredictivePowertrain {

DataManagement::DataManagement() {
	std::string fileRay[3] = {this->routeData, this->cityData, this->tripData};
	for(int i = 0; i < 3; i++)
	{
		std::ifstream dataFile(fileRay[i].c_str());
		if(!dataFile.good())
		{
			std::ofstream newFile(fileRay[i].c_str());
			newFile.close();
		}
		dataFile.close();
	}
}

void DataManagement::addRouteData(Route* route) {
}

void DataManagement::addCityData(City* city) {
	GenericMap<int, Road*>* roadMap = city->getRoads();
	GenericMap<int, Intersection*>* intersectionMap = city->getIntersections();
	GenericMap<int, Bounds*>* boundsMap = city->getBoundsMap();

	int boundsID = -1;
	bool newBounds = false;
	ptree cityLogs;
	try {
		read_json(this->cityData, cityLogs);
		BOOST_FOREACH(ptree::value_type& v, cityLogs)
		{
			boundsID = lexical_cast<int>(v.first.data());
			if(!boundsMap->hasEntry(boundsID))
			{
				newBounds = true;
			}
		}
	} catch(std::exception& e) {
		newBounds = true;
		std::cout << e.what() << std::endl;
	}

	if(newBounds)
	{
		ptree boundsData, roads_ptree, intersections_ptree;
		roadMap->initializeCounter();
		GenericEntry<int, Road*>* nextRoad = roadMap->nextEntry();
		while(nextRoad != NULL)
		{
			if(nextRoad->value->getBoundsID() != boundsID)
			{
				ptree road, startNode, endNode, roadType, nodes;
				startNode.put("", nextRoad->value->getStartIntersection()->getIntersectionID());
				endNode.put("", nextRoad->value->getEndIntersection()->getIntersectionID());
				roadType.put("", nextRoad->value->getRoadType());

				ptree lats, lons;
				nextRoad->value->getNodes()->initializeCounter();
				GenericEntry<int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
				while(nextNode != NULL)
				{
					Node* node = nextNode->value;
					ptree lat, lon;
					lat.put("", node->getLat());
					lon.put("", node->getLon());

					lats.push_back(std::make_pair("", lat));
					lons.push_back(std::make_pair("", lon));

					nextNode = nextRoad->value->getNodes()->nextEntry();
				}
				nodes.push_back(std::make_pair("latitude", lats));
				nodes.push_back(std::make_pair("longitude", lons));

				road.push_back(std::make_pair("startNodeID", startNode));
				road.push_back(std::make_pair("endNodeID", endNode));
				road.push_back(std::make_pair("roadType", roadType));
				road.push_back(std::make_pair("nodes", nodes));

				roads_ptree.add_child(lexical_cast<std::string>(nextRoad->value->getRoadID()), road);
			}
			nextRoad = roadMap->nextEntry();
		}

		intersectionMap->initializeCounter();
		GenericEntry<int, Intersection*>* nextIntersection = intersectionMap->nextEntry();
		while(nextIntersection != NULL)
		{
			if(nextIntersection->value->getBoudsID() != boundsID)
			{
				ptree intersection, roadCount, elevation, lat, lon;
				roadCount.put("", nextIntersection->value->getRoadCount());
				elevation.put("", nextIntersection->value->getElevation());
				lat.put("", nextIntersection->value->getLat());
				lon.put("", nextIntersection->value->getLon());

				ptree roadIDs;
				GenericMap<int, Road*>* connectingRoads = nextIntersection->value->getRoads();
				connectingRoads->initializeCounter();
				GenericEntry<int, Road*>* nextRoad = connectingRoads->nextEntry();
				while(nextRoad != NULL)
				{
					ptree roadID;
					roadID.put("", nextRoad->value->getRoadID());
					roadIDs.push_back(std::make_pair("", roadID));
					nextRoad = connectingRoads->nextEntry();
				}

				intersection.push_back(std::make_pair("roadCount", roadCount));
				intersection.push_back(std::make_pair("elevation", elevation));
				intersection.push_back(std::make_pair("latitude", lat));
				intersection.push_back(std::make_pair("longitude", lon));
				intersection.push_back(std::make_pair("roadIDs", roadIDs));

				intersections_ptree.add_child(lexical_cast<std::string>(nextIntersection->value->getIntersectionID()), intersection);
			}
			nextIntersection = intersectionMap->nextEntry();
		}

		boundsData.push_back(std::make_pair("roads", roads_ptree));
		boundsData.push_back(std::make_pair("intersections", intersections_ptree));
		cityLogs.add_child(lexical_cast<std::string>(boundsID+1), boundsData);
		write_json(this->cityData, cityLogs);
	}
}

void DataManagement::addTripData(GenericMap<double, double>* latLon, bool logSameDay) {

	int dayID = 0;
	ptree tripLogs;

	// check for existing trips
	try {
		read_json(this->tripData, tripLogs);
		BOOST_FOREACH(ptree::value_type &v, tripLogs)
		{
			dayID = lexical_cast<int>(v.first.data());
		}
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	// add trip
	ptree trip, allLat, allLon;
	latLon->initializeCounter();
	GenericEntry<double, double>* next = latLon->nextEntry();
	while(next != NULL)
	{
		ptree lat, lon;
		lat.put("", next->key);
		lon.put("", next->value);

		allLat.push_back(std::make_pair("", lat));
		allLon.push_back(std::make_pair("", lon));
		next = latLon->nextEntry();
	}

	trip.push_back(std::make_pair("latitude", allLat));
	trip.push_back(std::make_pair("longitude", allLon));

	if(!logSameDay) { dayID++; }

	tripLogs.add_child(lexical_cast<std::string>(dayID), trip);

	write_json(this->tripData, tripLogs);
}

GenericMap<int, Route*>* DataManagement::getRoutes(int cityClusterNUm) {
}

std::pair<GenericMap<int, Road*> *, GenericMap<int, Intersection*>*>* DataManagement::getCityData(int cityClusterNum) {
}

GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* DataManagement::getMostRecentTripData() {

	ptree tripLog;
	int dayID = 0;
	try {
		read_json(this->tripData, tripLog);
		BOOST_FOREACH(ptree::value_type& v, tripLog)
		{
			dayID = lexical_cast<int>(v.first.data());
		}

		GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* recentTripData = new GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>();
		int latCount = 0, lonCount = 0;
		BOOST_FOREACH(ptree::value_type& v, tripLog)
		{
			if(lexical_cast<int>(v.first.data()) == dayID)
			{
				GenericMap<int, double>* lats = new GenericMap<int, double>();
				GenericMap<int, double>* lons = new GenericMap<int, double>();
				BOOST_FOREACH(ptree::value_type& z, v.second)
				{
					std::string subTree = z.first.data();

					BOOST_FOREACH(ptree::value_type& b, z.second)
					{
						if(!subTree.compare("latitude"))
						{
							double lat = lexical_cast<double>(b.second.data());
							lats->addEntry(latCount++, lat);
							std::cout << subTree << " = " << lat << std::endl;
						} else if(!subTree.compare("longitude")) {
							double lon = lexical_cast<double>(b.second.data());
							lons->addEntry(lonCount++, lon);
							std::cout << subTree << " = " << lon << std::endl;
						}
					}

				}
				recentTripData->addEntry(lats, lons);
			}
		}
		return recentTripData;
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return NULL;
}

}





