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
		GenericMap<long int, Road*>* rawRoads = dc->makeRawRoads();

		// make splines for roads
		rawRoads->initializeCounter();
		GenericEntry<long int, Road*>* nextRawRoad = rawRoads->nextEntry();
		while(nextRawRoad != NULL)
		{
			GenericMap<long int, Node*>* nodes = nextRawRoad->value->getNodes();
			Eigen::MatrixXd points(2, nodes->getSize());
			int latLonCount = 0;

			nodes->initializeCounter();
			GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
			while(nextNode != NULL)
			{
				points(0, latLonCount) = nextNode->value->getLat();
				points(1, latLonCount) = nextNode->value->getLon();

				nextNode = nodes->nextEntry();
				latLonCount++;
			}
			free(nextNode);

			typedef Eigen::Spline<double, 2> spline2d;
			spline2d newSpline = Eigen::SplineFitting<spline2d>::Interpolate(points, 2);
			rawRoads->getEntry(nextRawRoad->key)->assignSpline(newSpline);

			nextRawRoad = rawRoads->nextEntry();
		}
		free(nextRawRoad);

		// find intersections

		// trim new roads
		// add roads, intersections, and bounds to city
		// record data
	}
}

}
