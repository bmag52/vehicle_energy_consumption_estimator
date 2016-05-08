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

std::pair<GenericMap<int, Intersection*> *, GenericMap<long int, Road*>*>* BuildCity::parseAdjMat() {

}

void BuildCity::updateGridData() {

	DataManagement dm;
	GenericMap<long int, std::pair<double, double>*>* tripLatLon = dm.getMostRecentTripData();
	City* city = dm.getCityData();
	int boundsID = 0;

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
				boundsID = bound->getID();
				if(lat > bound->getMaxLat() || lat < bound->getMinLat() || lon > bound->getMaxLon() || lon < bound->getMinLon())
				{
					this->newBounds = true;
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
		this->newBounds = true;
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

	if(this->newBounds)
	{
		std::cout << "identifying intersections" << std::endl;
		Bounds* newBoundsFromTrip = new Bounds(maxLat, minLat, maxLon, minLon);
		newBoundsFromTrip->assignID(boundsID+1);

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
		this->rawRoads = dc->makeRawRoads();

		int latRowsSpline = latDelta/this->adjMatPrecFromSplines;
		int lonColsSpline = lonDelta/this->adjMatPrecFromSplines;
		this->adjMatFromSplines = Eigen::MatrixXd::Zero(latRowsSpline, lonColsSpline);

		this->rawRoads->initializeCounter();
		GenericEntry<long int, Road*>* nextRawRoad = this->rawRoads->nextEntry();
		while(nextRawRoad != NULL)
		{
			// adjacent matrix of splines
			std::cout << "---- new road: " << nextRawRoad->key << " ----" << std::endl;

			bool splineWithinNodes = false;
			int latLonCount = 1;

			GenericMap<long int, Node*>* nodes = nextRawRoad->value->getNodes()->copy();
			GenericMap<int, std::pair<int, int>*>* adjMatIndicies = new GenericMap<int, std::pair<int, int>*>();


			Eigen::Spline<double,2> spline = nextRawRoad->value->getSpline();
			for(double u = 0; u <= 1; u += this->splineStep)
			{
				Eigen::Spline<double,2>::PointType point = spline(u);

				double newLat = point(0,0);
				double newLon = point(1,0);

				nodes->initializeCounter();
				GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
				while(nextNode != NULL)
				{
					// make sure spline is within node bounds
					if(	newLat + this->gpsTolerance > nextNode->value->getLat() &&
						newLat - this->gpsTolerance < nextNode->value->getLat() &&
						newLon + this->gpsTolerance > nextNode->value->getLon() &&
						newLon - this->gpsTolerance < nextNode->value->getLon() )
					{
						splineWithinNodes = true;
						nodes->erase(nextNode->key);
						break;
					}
					nextNode = nodes->nextEntry();
				}
				free(nextNode);

				// hop out of for loop if all nodes have been iterated over
				if(nodes->getSize() == 0) { break; }

				// add spline data to adjacency matrix if spline within node bounds
				if(splineWithinNodes)
				{
					int latRow = this->adjMatFromSplines.rows() - (newLat - minLat) / latDelta * latRowsSpline;
					int lonCol = (newLon - minLon) / lonDelta * lonColsSpline;

					if(latRow >= 0 && latRow < this->adjMatFromSplines.rows() && lonCol >= 0 && lonCol < this->adjMatFromSplines.cols())
					{
						std::cout << u << "\tlat: " << (double)point(0,0) << "\tlon: " << (double)point(1,0) << std::endl;
						adjMatIndicies->addEntry(latLonCount++, new std::pair<int, int>(latRow, lonCol));
						this->adjMatFromSplines(latRow, lonCol) = this->scaleID(nextRawRoad->key);
					}
				}
			}
			nextRawRoad->value->assignAdjMatIndicies(adjMatIndicies);
			nextRawRoad = this->rawRoads->nextEntry();
		}

		// fill in holes in the adjMat
		this->connectifyAjdMat();

		// parse out nodes
		std::pair<GenericMap<int, Intersection*>*, GenericMap<long int, Road*>*>* parsedData = this->parseAdjMat();
	}
}

void BuildCity::printAdjMats() {
	std::cout << "printing adjacency matrix from build city" << std::endl;
	if(this->hasNewBounds())
	{
		std::string csvName = "adjMatSpline.csv";

		// remove old csv
		std::string rm = "rm " + csvName;
		system(rm.c_str());

		// populate new csv
		std::ofstream csv;
		csv.open(csvName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
		for(int row = 0; row < this->adjMatFromSplines.rows(); row++)
		{
			for(int col = 0; col < this->adjMatFromSplines.cols(); col++)
			{
				csv << this->unScaleID((double)this->adjMatFromSplines(row, col)) << ",";
			}
			csv << "\n";
		}
		csv.close();
	} else {
		std::cout << "no new bounds" << std::endl;
	}
}

void BuildCity::connectifyAjdMat() {
	this->rawRoads->initializeCounter();
	GenericEntry<long int, Road*>* nextRawRoad = this->rawRoads->nextEntry();
	while(nextRawRoad != NULL)
	{
		GenericMap<int, std::pair<int, int>*>* currIndicies = nextRawRoad->value->getAdjMatIndicies();

		currIndicies->initializeCounter();
		GenericEntry<int, std::pair<int, int>*>* currIdx = currIndicies->nextEntry();
		GenericEntry<int, std::pair<int, int>*>* nextIdx = currIndicies->nextEntry();

		while(nextIdx != NULL)
		{
			if(!this->isAdj(currIdx, nextIdx))
			{
				int fillCount = 0;
				GenericEntry<int, std::pair<int, int>*>* fillIdx = new GenericEntry<int, std::pair<int, int>*>(1, currIdx->value);
				while(!this->isAdj(fillIdx, nextIdx))
				{
					// adjust x
					int currX = fillIdx->value->first;
					int currY = fillIdx->value->second;
					int nextX = nextIdx->value->first;
					int nextY = nextIdx->value->second;

					if(currX < nextX)
					{
						fillIdx->value->first += 1;
					} else if(currX < nextX) {
						fillIdx->value->first -= 1;
					}

					// adjust y
					if(currY < nextY)
					{
						fillIdx->value->second += 1;
					} else if(currY > nextY) {
						fillIdx->value->second -= 1;
					}
					this->adjMatFromSplines(fillIdx->value->first, fillIdx->value->second) = this->scaleID(nextRawRoad->key);

					// incase going to HAM connectifying
					if(fillCount++ > 100)
					{
						break;
					}
				}
			}
			currIdx = nextIdx;
			nextIdx = currIndicies->nextEntry();
		}
		free(currIdx);
		free(nextIdx);

		nextRawRoad = this->rawRoads->nextEntry();
	}
	free(nextRawRoad);
}

bool BuildCity::isAdj(GenericEntry<int, std::pair<int, int>*>* idx1, GenericEntry<int, std::pair<int, int>*>* idx2) {
	int x1 = idx1->value->first;
	int y1 = idx1->value->second;
	int x2 = idx2->value->first;
	int y2 = idx2->value->second;

	return abs(x1-x2) <= 1 && abs(y1-y2) <= 1;
}

double BuildCity::scaleID(long int id) {
	return id / this->idScalar;
}

long int BuildCity::unScaleID(double id) {
	return id * this->idScalar;
}

bool BuildCity::hasNewBounds() {
	return this->newBounds;
}

}
