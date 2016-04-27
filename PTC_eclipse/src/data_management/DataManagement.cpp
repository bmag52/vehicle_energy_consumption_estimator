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
	GenericMap<int, Bounds*>* newBoundsMap = new GenericMap<int, Bounds*>();

	bool newBounds = false;
	ptree cityLogs;
	try {
		read_json(this->cityData, cityLogs);

		boundsMap->initializeCounter();
		GenericEntry<int, Bounds*>* nextBounds = boundsMap->nextEntry();
		while(nextBounds != NULL)
		{
			bool boundsNotLogged = true;
			BOOST_FOREACH(ptree::value_type& v, cityLogs)
			{
				int boundsID = lexical_cast<int>(v.first.data());
				if(boundsID == nextBounds->key)
				{
					boundsNotLogged = false;
					break;
				}
			}
			if(boundsNotLogged) {
				newBounds = true;
				newBoundsMap->addEntry(nextBounds->key, NULL);
			}
			nextBounds = boundsMap->nextEntry();
		}
	} catch(std::exception& e) {
		std::cout << e.what() << std::endl;
		newBoundsMap = boundsMap;
		newBounds = true;
	}

	if(newBounds)
	{
		newBoundsMap->initializeCounter();
		GenericEntry<int, Bounds*>* nextBounds = newBoundsMap->nextEntry();
		while(nextBounds != NULL)
		{
			ptree boundsData, roads_ptree, intersections_ptree;
			roadMap->initializeCounter();
			GenericEntry<int, Road*>* nextRoad = roadMap->nextEntry();
			while(nextRoad != NULL)
			{
				if(nextRoad->value->getBoundsID() == nextBounds->key)
				{
					ptree road, startNode, endNode, roadType, nodes;
					startNode.put("", nextRoad->value->getStartIntersection()->getIntersectionID());
					endNode.put("", nextRoad->value->getEndIntersection()->getIntersectionID());
					roadType.put("", nextRoad->value->getRoadType());

					ptree lats, lons, nodeElevations, nodeIDs;
					nextRoad->value->getNodes()->initializeCounter();
					GenericEntry<int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
					while(nextNode != NULL)
					{
						Node* node = nextNode->value;
						ptree lat, lon, ele, id;
						lat.put("", node->getLat());
						lon.put("", node->getLon());
						ele.put("", node->getEle());
						id.put("", node->getID());

						lats.push_back(std::make_pair("", lat));
						lons.push_back(std::make_pair("", lon));
						nodeElevations.push_back(std::make_pair("", ele));
						nodeIDs.push_back(std::make_pair("", id));

						nextNode = nextRoad->value->getNodes()->nextEntry();
					}
					nodes.push_back(std::make_pair("latitude", lats));
					nodes.push_back(std::make_pair("longitude", lons));
					nodes.push_back(std::make_pair("elevation", nodeElevations));
					nodes.push_back(std::make_pair("nodeIDs", nodeIDs));

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
				if(nextIntersection->value->getBoudsID() == nextBounds->key)
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

			ptree bounds, maxLat, maxLon, minLat, minLon;
			maxLat.put("", boundsMap->getEntry(nextBounds->key)->getMaxLat());
			maxLon.put("", boundsMap->getEntry(nextBounds->key)->getMaxLon());
			minLat.put("", boundsMap->getEntry(nextBounds->key)->getMinLat());
			minLon.put("", boundsMap->getEntry(nextBounds->key)->getMinLon());

			bounds.push_back(std::make_pair("maxLat", maxLat));
			bounds.push_back(std::make_pair("maxLon", maxLon));
			bounds.push_back(std::make_pair("minLat", minLat));
			bounds.push_back(std::make_pair("minLon", minLon));

			boundsData.push_back(std::make_pair("roads", roads_ptree));
			boundsData.push_back(std::make_pair("intersections", intersections_ptree));
			boundsData.push_back(std::make_pair("bounds", bounds));

			cityLogs.add_child(lexical_cast<std::string>(nextBounds->key), boundsData);
			nextBounds = newBoundsMap->nextEntry();
		}
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

std::tuple<GenericMap<int, Road*>*, GenericMap<int, Intersection*>*, GenericMap<int, Bounds*>*>* DataManagement::getCityData(int cityClusterNum) {

	ptree cityLogs;
	try {
		read_json(this->cityData, cityLogs);
		GenericMap<int, Road*>* roads = new GenericMap<int, Road*>();
		GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
		GenericMap<int, Bounds*>* bounds = new GenericMap<int, Bounds*>();
		GenericMap<int, std::pair<int, int>*>* roadIntersections = new GenericMap<int, std::pair<int, int>*>(); // <roadID, <startID, endID>>

		// get roads first
		BOOST_FOREACH(ptree::value_type& u, cityLogs)
		{
			int boundsID = lexical_cast<int>(u.first.data());
			BOOST_FOREACH(ptree::value_type& v, u.second)
			{
				std::string child = v.first.data();
				if(!child.compare("roads")) {

					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						int roadID = lexical_cast<int>(z.first.data());
						int startNodeID, endNodeID;
						GenericMap<int, double>* nodeLats = new GenericMap<int, double>();
						GenericMap<int, double>* nodeLons = new GenericMap<int, double>();
						GenericMap<int, int>* nodeEles = new GenericMap<int, int>();
						GenericMap<int, long int>* nodeIDs = new GenericMap<int, long int>();
						std::string roadType;
						BOOST_FOREACH(ptree::value_type& a, z.second)
						{
							std::string roadFeature = a.first.data();
							if(!roadFeature.compare("startNodeID")) {
								startNodeID = lexical_cast<int>(a.second.data());
							} else if(!roadFeature.compare("endNodeID")) {
								endNodeID = lexical_cast<int>(a.second.data());
							} else if(!roadFeature.compare("roadType")) {
								roadType = a.second.data();
							} else if(!roadFeature.compare("nodes")) {
								int latCount = 0;
								int lonCount = 0;
								int eleCount = 0;
								int idCount = 0;
								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									std::string nodeFeature = b.first.data();
									BOOST_FOREACH(ptree::value_type& c, b.second)
									{
										if(!nodeFeature.compare("latitude")) {
											nodeLats->addEntry(latCount++, lexical_cast<double>(b.second.data()));
										} else if(!nodeFeature.compare("longitude")) {
											nodeLons->addEntry(lonCount++, lexical_cast<double>(b.second.data()));
										} else if(!nodeFeature.compare("elevation")) {
											nodeEles->addEntry(eleCount++, lexical_cast<int>(b.second.data()));
										} else if(!nodeFeature.compare("nodeIDs")) {
											nodeIDs->addEntry(idCount++, lexical_cast<long int>(b.second.data()));
										}
									}
								}
							}
						}

						GenericMap<int, Node*>* nodes = new GenericMap<int, Node*>();
						for(int i = 1; i <= nodeLats->getSize(); i++)
						{
							double lat = nodeLats->getEntry(i);
							double lon = nodeLons->getEntry(i);
							int ele = nodeEles->getEntry(i);
							int id = nodeIDs->getEntry(i);

							nodes->addEntry(i, new Node(lat, lon, ele, id));
						}

						roadIntersections->addEntry(roadID, new std::pair<int, int>(startNodeID, endNodeID));
						roads->addEntry(roadID, new Road(roadType, roadID, nodes));

					}
				}
			}
		}

		// get intersections and bounds next adding roads to the intersections
		BOOST_FOREACH(ptree::value_type& u, cityLogs)
		{
			int boundsID = lexical_cast<int>(u.first.data());
			BOOST_FOREACH(ptree::value_type& v, u.second)
			{
				std::string child = v.first.data();
				if(!child.compare("intersections")) {
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						int intersectionID = lexical_cast<int>(z.first.data());
						BOOST_FOREACH(ptree::value_type& a, z.second)
						{
							std::string intFeature = a.first.data();
							if(!intFeature.compare("roadCount")) {

							} else if(!intFeature.compare("elevation")) {

							} else if(!intFeature.compare("latitude")) {

							} else if(!intFeature.compare("longitude")) {

							} else if(!intFeature.compare("roadIDs")) {
								BOOST_FOREACH(ptree::value_type& b, a.second)
								{

								}
							}
						}
					}
				} else if(!child.compare("bounds")) {
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						std::string type = z.first.data();
						if(!type.compare("maxLat")) {

						} else if(!type.compare("maxLon")) {

						} else if(!type.compare("minLat")) {

						} else if(!type.compare("minLon")) {

						}
					}
				}
			}
		}
		free(roadIntersections);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
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





