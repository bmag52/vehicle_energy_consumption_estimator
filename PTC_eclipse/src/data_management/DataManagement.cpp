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
	// check files

}

void DataManagement::addRouteData(Route* route) {
}

void DataManagement::addCityData(Road* roads, Intersection* intersections) {
}

void DataManagement::addTripData(GenericMap<double, double>* latLon) {
	// city >> arrary (include time or count) >> data

	ptree tripLog;
	read_json(this->tripData, tripLog);

	int tripNumber;
	BOOST_FOREACH(ptree::value_type &v, tripLog)
	{
		tripNumber = lexical_cast<int>(v.first.data());
	}

	ptree trip, lat, lon;
	latLon->initializeCounter();
	GenericEntry<double, double>* next = latLon->nextEntry();
	while(next != NULL)
	{
		lat.put("", next->key);
		lon.put("", next->value);
		next = latLon->nextEntry();
	}
	trip.put("", tripNumber+1);
	trip.push_back(std::make_pair("latitude", lat));
	trip.push_back(std::make_pair("longitude", lon));
	tripLog.push_back(std::make_pair("", trip));

	write_json(this->tripData, tripLog);
}

GenericMap<int, Route*>* DataManagement::getRoutes(int cityClusterNUm) {
}

std::pair<GenericMap<int, Road*> *, GenericMap<int, Intersection*> *>* DataManagement::getCityData(int cityClusterNum) {
}

std::pair<double*, double*>* DataManagement::getMostRecentTripData() {
}

}





