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

void DataManagement::addCityData(Road* roads, Intersection* intersections) {
}

void DataManagement::addTripData(GenericMap<double, double>* latLon, bool logSameDay) {

	int dayID = 0;
	ptree tripLog;

	// check for existing trips
	try {
		read_json(this->tripData, tripLog);
		BOOST_FOREACH(ptree::value_type &v, tripLog)
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

	tripLog.add_child(lexical_cast<std::string>(dayID), trip);

	write_json(this->tripData, tripLog);
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





