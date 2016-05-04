/*
 * BuildCity.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "BuildCity.h"

namespace PredictivePowertrain {

BuildCity::BuildCity() {
}

void BuildCity::updateGridData() {

	DataManagement dm;
	GenericMap<long int, std::pair<double, double>*>* tripLatLon = dm.getMostRecentTripData();
	City* city = dm.getCityData();

	bool newBounds = false;
	double maxLat = -DBL_MAX;
	double maxLon = -DBL_MAX;
	double minLat = DBL_MAX;
	double minLon = DBL_MAX;

	tripLatLon->initializeCounter();
	GenericEntry<long int, std::pair<double, double>*>* nextTripLatLon = tripLatLon->nextEntry();
	if(city != NULL)
	{
		// bounds data already exists
		GenericMap<int, Bounds*>* bounds = city->getBoundsMap();
		while(nextTripLatLon != NULL)
		{
			bounds->initializeCounter();
			GenericEntry<int, Bounds*>* nextBounds = bounds->nextEntry();
			while(nextBounds != NULL)
			{
				double lat = nextTripLatLon->value->first;
				double lon = nextTripLatLon->value->second;
				Bounds* bound = nextBounds->value;
				if(lat > bound->getMaxLat() || lat < bound->getMinLat() || lon > bound->getMaxLon() || lon < bound->getMinLon())
				{
					newBounds = true;
					if(lat > maxLat) { maxLat = lat; } if(lat < minLat) { minLat = lat; }
					if(lon > maxLon) { maxLon = lon; } if(lon < minLon) { minLon = lon; }

				}
				nextBounds = bounds->nextEntry();
			}
			nextTripLatLon = tripLatLon->nextEntry();
			free(nextBounds);
		}
	} else {
		// no bounds data
		newBounds = true;
		while(nextTripLatLon != NULL)
		{
			double lat = nextTripLatLon->value->first;
			double lon = nextTripLatLon->value->second;

			if(lat > maxLat) { maxLat = lat; } if(lat < minLat) { minLat = lat; }
			if(lon > maxLon) { maxLon = lon; } if(lon < minLon) { minLon = lon; }
			nextTripLatLon = tripLatLon->nextEntry();
		}
	}
	free(nextTripLatLon);

	if(newBounds)
	{
		double latCenter = (maxLat + minLat) / 2.0;
		double lonCenter = (maxLon + minLon) / 2.0;
		double latDelta = maxLat - minLat;
		double lonDelta = maxLon - minLon;

		DataCollection* dc;
		if(latDelta == 0 && lonDelta == 0)
		{
			dc = new DataCollection();
		} else {
			dc = new DataCollection(latDelta, lonDelta);
		}
		dc->pullData(latCenter, lonCenter);
		GenericMap<long int, Road*>* rawRoads1 = dc->makeRawRoads();
		GenericMap<long int, Road*>* rawRoads2 = dc->makeRawRoads();

		// find intersections
		GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
		GenericMap<int, Intersection*>* ints = new GenericMap<int, Intersection*>();

		// trim new roads
		// add roads, intersections, and bounds to city
		// record data
	}
}

}
