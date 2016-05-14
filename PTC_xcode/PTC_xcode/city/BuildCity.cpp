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
    // TODO yeah uhhh no thanks
    return NULL;
}

void BuildCity::updateGridDataXML() {
    
    if(this->hasNewBounds())
    {
        std::cout << "identifying intersections from XML" << std::endl;
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        Bounds* newBounds = newMapData->second;
        
        dc->pullDataXML(this->latCenter, this->lonCenter);
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
                    int latRow = this->adjMatFromSplines.rows() - (newLat - this->minLat) / this->latDelta * latRowsSpline;
                    int lonCol = (newLon - this->minLon) / this->lonDelta * lonColsSpline;
                    
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

void BuildCity::updateGridDataPNG() {
    if(this->hasNewBounds())
    {
        std::cout << "identifying intersections from PNG" << std::endl;
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        Bounds* newBounds = newMapData->second;
        
        cv::Mat roadGrid = pullAndFormatMapPNG(dc);
    }
}

cv::Mat BuildCity::pullAndFormatMapPNG(DataCollection* dc) {
    
    // pull in image
    int zoomIdx = dc->pullDataPNG(this->latCenter, this->lonCenter);
    std::string dataFolder = dc->getDataFolder();
    std::string mapPNGName = dc->getMapPNGName();
    std::string mapPicLoc = dataFolder + "/" + mapPNGName;
    
    // get raw image
    cv::Mat mapImage = cv::imread(mapPicLoc, CV_LOAD_IMAGE_COLOR);
    
    // trim raw image
    cv::Rect roi(350, 55, 5650, 5945);
    cv::Mat croppedMapImage = mapImage(roi);
    
    // red highway color filter
    cv::Mat redMask;
    cv::Scalar redLB = cv::Scalar(80, 65, 190);
    cv::Scalar redUB = cv::Scalar(170, 147, 251);
    cv::inRange(croppedMapImage, redLB, redUB, redMask);
    
    // yellow access road color filter
    cv::Mat yellowMask;
    cv::Scalar yellowLB = cv::Scalar(117, 223, 228);
    cv::Scalar yellowUB = cv::Scalar(195, 270, 251);
    cv::inRange(croppedMapImage, yellowLB, yellowUB, yellowMask);
    
    // white residential road color filter
    cv::Mat whiteMask;
    cv::Scalar whiteLB = cv::Scalar(250, 250, 250);
    cv::Scalar whiteUB = cv::Scalar(254, 254, 254);
    cv::inRange(croppedMapImage, whiteLB, whiteUB, whiteMask);
    
    cv::imwrite("/Users/Brian/Desktop/misc/test.png", redMask + yellowMask + whiteMask);
    return croppedMapImage;
    
}

std::pair<DataCollection*, Bounds*>* BuildCity::setupDataCollection() {
    
    Bounds* newBoundsFromTrip = new Bounds(this->maxLat, this->minLat, this->maxLon, this->minLon);
    newBoundsFromTrip->assignID(this->boundsID+1);
    
    this->latCenter = (this->maxLat + this->minLat) / 2.0;
    this->lonCenter = (this->maxLon + this->minLon) / 2.0;
    this->latDelta = this->maxLat - this->minLat;
    this->lonDelta = this->maxLon - this->minLon;
    
    DataCollection* dc;
    if(this->latDelta == 0 && this->lonDelta == 0)
    {
        dc = new DataCollection();
    } else {
        dc = new DataCollection(this->latDelta, this->lonDelta);
    }
    
    return new std::pair<DataCollection*, Bounds*>(dc, newBoundsFromTrip);
}

bool BuildCity::hasNewBounds() {
    DataManagement dm;
    GenericMap<long int, std::pair<double, double>*>* tripLatLon = dm.getMostRecentTripData();
    City* city = dm.getCityData();
    
    this->maxLat = -DBL_MAX;
    this->maxLon = -DBL_MAX;
    this->minLat = DBL_MAX;
    this->minLon = DBL_MAX;
    
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
                this->boundsID = bound->getID();
                if(lat > bound->getMaxLat() || lat < bound->getMinLat() || lon > bound->getMaxLon() || lon < bound->getMinLon())
                {
                    this->newBounds = true;
                    if(lat > this->maxLat) { this->maxLat = lat; } if(lat < this->minLat) { this->minLat = lat; }
                    if(lon > this->maxLon) { this->maxLon = lon; } if(lon < this->minLon) { this->minLon = lon; }
                    
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
            
            if(lat > this->maxLat) { this->maxLat = lat; } if(lat < this->minLat) { this->minLat = lat; }
            if(lon > this->maxLon) { this->maxLon = lon; } if(lon < this->minLon) { this->minLon = lon; }
            nextTripLatLon = tripLatLon->nextEntry();
        }
    }
    free(nextTripLatLon);
    
    return this->newBounds;
}
} // end of predictivepowertrain