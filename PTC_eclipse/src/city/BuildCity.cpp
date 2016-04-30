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
	GenericMap<double, double>* latLons = dm.getMostRecentTripData();
	GenericMap<int, Bounds*>* bounds = dm.getCityData()->getBoundsMap();

	bool newBounds = false;
	double maxLat = -DBL_MAX;
	double maxLon = -DBL_MAX;
	double minLat = DBL_MAX;
	double minLon = DBL_MAX;

	latLons->initializeCounter();
	GenericEntry<double, double>* nextLatLon = latLons->nextEntry();
	while(nextLatLon != NULL)
	{
		bounds->initializeCounter();
		GenericEntry<int, Bounds*>* nextBounds = bounds->nextEntry();
		while(nextBounds != NULL)
		{
			double lat = nextLatLon->key;
			double lon = nextLatLon->value;
			Bounds* bound = nextBounds->value;
			if(lat > bound->getMaxLat() || lat < bound->getMinLat() || lon > bound->getMaxLon() || lon < bound->getMinLon())
			{
				newBounds = true;
				if(lat > maxLat) { maxLat = lat; }
				if(lat < minLat) { minLat = lat; }
				if(lon > maxLon) { maxLon = lon; }
				if(lon < minLon) { minLon = lon; }

			}
			nextBounds = bounds->nextEntry();
		}
		nextLatLon = latLons->nextEntry();
		free(nextBounds);
	}
	free(nextLatLon);

	if(newBounds)
	{
		double latCenter = (maxLat + minLat) / 2.0;
		double lonCenter = (maxLon + minLon) / 2.0;
		double latDelta = (maxLat - minLat) / 2.0;
		double lonDelta = (maxLon - minLon) / 2.0;

		DataCollection* dc;
		if(latDelta == 0 && lonDelta == 0)
		{
			dc = new DataCollection();
		} else {
			dc = new DataCollection(latCenter, lonCenter);
		}
		dc->pullData(latCenter, lonCenter);
		GenericMap<long int, Road*>* rawRoads = dc->makeRawRoads();

		// make splines for roads
		rawRoads->initializeCounter();
		GenericEntry<long int, Road*>* nextRawRoad = rawRoads->nextEntry();
		while(nextRawRoad != NULL)
		{
			GenericMap<long int, Node*>* nodes = nextRawRoad->value->getNodes();
			std::vector<double> lats(nodes->getSize()), lons(nodes->getSize());
			int latLonCount = 0;

			nodes->initializeCounter();
			GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
			while(nextNode != NULL)
			{
				lats[latLonCount] = nextNode->value->getLat();
				lons[latLonCount] = nextNode->value->getLon();
				nextNode = nodes->nextEntry();
			}
			free(nextNode);

			tk::spline* spline = new tk::spline();
			spline->set_points(lats, lons);
//			rawRoads->getEntry(nextRawRoad->key)->assignSpline(spline);

			nextRawRoad = rawRoads->nextEntry();
		}
		free(nextRawRoad);
		// find intersections
		// trip new roads
		// add roads, intersections, and bounds to city
		// record data
	}
}

}
