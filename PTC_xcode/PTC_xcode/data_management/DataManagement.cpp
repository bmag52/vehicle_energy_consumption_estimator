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
	GenericMap<int, Link*> * links = route->getLinks();
	// go through links map, get NN value from link
	// get 3 matricies which are JSONified to make them appear correctly
	Goal * goal = route->getGoal();

	ptree routeLogs;

	int routeID = 0;
	try {
		read_json(this->routeData, routeLogs);
		BOOST_FOREACH(ptree::value_type& v, routeLogs) {
			routeID = lexical_cast<int>(v.first.data());
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	ptree newRoute, newLinks;
	links->initializeCounter();
	GenericEntry<int, Link*> * next = links->nextEntry();
	while (next != NULL) {
		ptree link;
		int num = next->key;
		Link * nextLink = next->value;


		link.put("", next->value);
		newLinks.push_back(std::make_pair(lexical_cast<std::string>(num), link));
	}

	newRoute.push_back(std::make_pair("links", newLinks));

	ptree newGoal, numSeenTree;
	// do we need all this info about goals/do we need more?
	int numSeen = goal->getNumSeen();
	numSeenTree.put("", numSeen);
	newGoal.push_back(std::make_pair("numSeen", numSeenTree));

	newRoute.push_back(std::make_pair("goal", newGoal));

	routeID++;
	routeLogs.add_child(lexical_cast<std::string>(routeID), newRoute);
	write_json(this->routeData, routeLogs);
}

void DataManagement::addCityData(City* city) {
	GenericMap<long int, Road*>* roadMap = city->getRoads();
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
			GenericEntry<long int, Road*>* nextRoad = roadMap->nextEntry();
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
					GenericEntry<long int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
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
					GenericMap<long int, Road*>* connectingRoads = nextIntersection->value->getRoads();
					connectingRoads->initializeCounter();
					GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
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

void DataManagement::addTripData(GenericMap<long int, std::pair<double, double>*>* latLon) {

	int tripID = 0;
	ptree tripLogs;

	// check for existing trips
	try {
		read_json(this->tripData, tripLogs);
		BOOST_FOREACH(ptree::value_type &v, tripLogs)
		{
			tripID = lexical_cast<int>(v.first.data());
		}
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	// add trip
	ptree trip, allLat, allLon;
	latLon->initializeCounter();
	GenericEntry<long int, std::pair<double, double>*>* nextLatLon = latLon->nextEntry();
	while(nextLatLon != NULL)
	{
		ptree lat, lon;
		lat.put("", nextLatLon->value->first);
		lon.put("", nextLatLon->value->second);

		allLat.push_back(std::make_pair("", lat));
		allLon.push_back(std::make_pair("", lon));
		nextLatLon = latLon->nextEntry();
	}

	trip.push_back(std::make_pair("latitude", allLat));
	trip.push_back(std::make_pair("longitude", allLon));

	tripLogs.add_child(lexical_cast<std::string>(tripID+1), trip);

	write_json(this->tripData, tripLogs);
}


GenericMap<int, Route*>* DataManagement::getRoutes() {
	// what do i do with cityclusternum???

	ptree routeLogs;
	try {
		read_json(this->routeData, routeLogs);
		BOOST_FOREACH(ptree::value_type &v, routeLogs)
		{
			GenericMap<int, Link*> * links = new GenericMap<int, Link*>();

			/*
			 *
			 *
			 */
		}
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
    // TODO
    return NULL;
}

City* DataManagement::getCityData() {

	ptree cityLogs;
	try {
		read_json(this->cityData, cityLogs);
		GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
		GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
		GenericMap<int, Bounds*>* bounds = new GenericMap<int, Bounds*>();
		GenericMap<long int, std::pair<int, int>*>* roadIntersections = new GenericMap<long int, std::pair<int, int>*>(); // <roadID, <startID, endID>>

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
						GenericMap<int, double>* nodeLats = new GenericMap<int, double>();
						GenericMap<int, double>* nodeLons = new GenericMap<int, double>();
						GenericMap<int, int>* nodeEles = new GenericMap<int, int>();
						GenericMap<int, long int>* nodeIDs = new GenericMap<int, long int>();

						long int roadID = lexical_cast<long int>(z.first.data());
						int startNodeID, endNodeID;
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
								int latCount = 0; int lonCount = 0; int eleCount = 0; int idCount = 0;
								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									std::string nodeFeature = b.first.data();
									BOOST_FOREACH(ptree::value_type& c, b.second)
									{
										if(!nodeFeature.compare("latitude")) {
											nodeLats->addEntry(latCount++, lexical_cast<double>(c.second.data()));
										} else if(!nodeFeature.compare("longitude")) {
											nodeLons->addEntry(lonCount++, lexical_cast<double>(c.second.data()));
										} else if(!nodeFeature.compare("elevation")) {
											nodeEles->addEntry(eleCount++, lexical_cast<int>(c.second.data()));
										} else if(!nodeFeature.compare("nodeIDs")) {
											nodeIDs->addEntry(idCount++, lexical_cast<long int>(c.second.data()));
										}
									}
								}
							}
						}

						GenericMap<long int, Node*>* nodes = new GenericMap<long int, Node*>();
						for(int i = 1; i <= nodeLats->getSize(); i++)
						{

							nodes->addEntry(i, new Node(nodeLats->getEntry(i), nodeLons->getEntry(i), nodeEles->getEntry(i), nodeIDs->getEntry(i)));
						}
						delete(nodeLats); delete(nodeLons); delete(nodeEles); delete(nodeIDs);

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
						int intID = lexical_cast<int>(z.first.data());
						GenericMap<long int, Road*>* intRoads = new GenericMap<long int, Road*>();
						int ele; double lat, lon;
						BOOST_FOREACH(ptree::value_type& a, z.second)
						{
							std::string intFeature = a.first.data();
							if(!intFeature.compare("elevation")) {
								ele = lexical_cast<int>(a.second.data());
							} else if(!intFeature.compare("latitude")) {
								lat = lexical_cast<double>(a.second.data());
							} else if(!intFeature.compare("longitude")) {
								lon = lexical_cast<double>(a.second.data());
							} else if(!intFeature.compare("roadIDs")) {

								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									long int roadID = lexical_cast<long int>(b.second.data());
									intRoads->addEntry(roadID, roads->getEntry(roadID));
								}
							}
						}
						Intersection* newInt = new Intersection(intRoads, lat, lon, ele, intID);
						newInt->setBoundsID(boundsID);
						intersections->addEntry(intID, newInt);
					}
				} else if(!child.compare("bounds")) {
					double maxLat, maxLon, minLat, minLon;
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						std::string boundsFeature = z.first.data();
						if(!boundsFeature.compare("maxLat")) {
							maxLat = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("maxLon")) {
							maxLon = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("minLat")) {
							minLat = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("minLon")) {
							minLon = lexical_cast<double>(z.second.data());
						}
					}
					Bounds* newBounds = new Bounds(maxLat, maxLon, minLat, minLon);
					newBounds->assignID(boundsID);
					bounds->addEntry(boundsID, newBounds);
				}
			}
		}

		roadIntersections->initializeCounter();
		GenericEntry<long int, std::pair<int, int>*>* nextRoadInts = roadIntersections->nextEntry();
		while(nextRoadInts != NULL)
		{
			roads->getEntry(nextRoadInts->key)->setStartIntersection(intersections->getEntry(nextRoadInts->value->first));
			roads->getEntry(nextRoadInts->key)->setEndIntersection(intersections->getEntry(nextRoadInts->value->second));
			nextRoadInts = roadIntersections->nextEntry();
		}

		delete(roadIntersections);
		return new City(intersections, roads, bounds);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return NULL;
	}
}

GenericMap<long int, std::pair<double, double>*>* DataManagement::getMostRecentTripData() {
    
    std::cout << "getting most recent trip log data" << std::endl;

	ptree tripLog;
	int dayID = 0;
	try {
		read_json(this->tripData, tripLog);
		BOOST_FOREACH(ptree::value_type& v, tripLog)
		{
			dayID = lexical_cast<int>(v.first.data());
		}

		GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* rawRecentTripData = new GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>();
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
				assert(lats->getSize() == lons->getSize());
				rawRecentTripData->addEntry(lats, lons);
			}
		}

		GenericMap<long int, std::pair<double, double>*>* recentTripData = new GenericMap<long int, std::pair<double, double>*>();
		long int tripCount = 1;

		rawRecentTripData->initializeCounter();
		GenericEntry<GenericMap<int, double>*, GenericMap<int, double>*>* nextLatLonSet = rawRecentTripData->nextEntry();
		while(nextLatLonSet != NULL)
		{
			nextLatLonSet->key->initializeCounter();
			nextLatLonSet->value->initializeCounter();
			GenericEntry<int, double>* nextLat = nextLatLonSet->key->nextEntry();
			GenericEntry<int, double>* nextLon = nextLatLonSet->value->nextEntry();
			while(nextLat != NULL && nextLon != NULL)
			{
				recentTripData->addEntry(tripCount++, new std::pair<double, double>(nextLat->value, nextLon->value));
				nextLat = nextLatLonSet->key->nextEntry();
				nextLon = nextLatLonSet->value->nextEntry();
			}
			delete(nextLat);
			delete(nextLon);
			nextLatLonSet = rawRecentTripData->nextEntry();
		}
		delete(nextLatLonSet);
		delete(rawRecentTripData);

		return recentTripData;
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return NULL;
}

}





