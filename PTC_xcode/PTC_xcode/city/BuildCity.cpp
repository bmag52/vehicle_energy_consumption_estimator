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
        
        std::pair<int, cv::Mat>* newGridData = pullAndFormatMapPNG(dc);
        int zoomIdx = newGridData->first;
        cv::Mat map = newGridData->second;
    
        int kernelSideDim = 75;
        
//        for(int row = 0; row < map.rows; row += kernelSideDim)
//        {
//            std::cout << "row: " << row << std::endl;
//            for(int col = 0; col < map.cols; col += kernelSideDim)
//            {
//                std::cout << "col: " << col << std::endl;
//                if(row + kernelSideDim < map.rows && col + kernelSideDim < map.cols)
//                {
//                    cv::Rect roi(col, row, kernelSideDim, kernelSideDim);
//                    cv::Mat kernel = map(roi);
//                    
//                    perimeterScanKernelForRoads(kernel);
//                }
//            }
//        }
        perimeterScanKernelForRoads(map);

        
        // Display the detected hough lines
        cv::imshow("incoming road dots", map);
        cv::imwrite("/Users/Brian/Desktop/misc/kernelDev.png", map);
        
        
        // 1 slide a kernel along road to find intersections (
        //      perimeter scanning kernel to identify incoming roads crossing perimeter
        //      if two road inlets detected and scan back to center, no intersection detected
        //      if three or more inlets detected and scan back to center, intersection detected
        //      motion along previous road direction and update direction only after new trajectory conects back to center
        // 2 once intersection found, add it to map with connecting road or update connecting road if intersection exists
        // 3 add road to map with connecting intersection or update intersection if road exists
        // repeat 1-3 on in direction of other roads

    }
}
    
GenericMap<int, std::pair<cv::Point*, cv::Point*>*>* BuildCity::perimeterScanKernelForRoads(cv::Mat kernel) {
    
    GenericMap<int, cv::Point*> points;
    
    int lastPixel = kernel.at<uchar>(0, 0);
    
    // top edge
    std::cout << "top edge" << std::endl;
    for(int i = 1; i < kernel.cols; i++)
    {
        checkNextPixel(i, 100, points, kernel, lastPixel);
    }
    
    // right edge
    std::cout << "right edge" << std::endl;
    for(int i = 0; i < kernel.rows; i++)
    {
        checkNextPixel(kernel.cols-1, i, points, kernel, lastPixel);
    }
   
    // bottom edge
    std::cout << "bottom edge" << std::endl;
    for(int i = kernel.cols-1; i >= 0; i--)
    {
        checkNextPixel(i, kernel.rows-1, points, kernel, lastPixel);
    }
    
    // left edge
    std::cout << "left edge" << std::endl;
    for(int i = kernel.rows-1; i >= 0; i--)
    {
        checkNextPixel(0, i, points, kernel, lastPixel);
    }
    
    if(points.getSize() > 0)
    {
        cv::imshow("kernel", kernel);
    }
    return NULL;
}
    
void BuildCity::checkNextPixel(int x, int y, GenericMap<int, cv::Point*>& points, cv::Mat& kernel, int& lastPixel){
    int nextPixel = kernel.at<uchar>(x, y);
    
    std::cout << lastPixel << " " << nextPixel << std::endl;
    
    // found difference
    if(nextPixel != lastPixel)
    {
        cv::Point* newPt = new cv::Point(x, y);
        drawPoint(kernel, *newPt);
        points.addEntry(points.getSize()+1, newPt);
//        cv::imshow("test", kernel);
    }
    lastPixel = nextPixel;
}
    
// Draw the detected intersection point on an image
void BuildCity::drawPoint(cv::Mat &image, cv::Point point) {
  
    int rad = 7;
    cv::Scalar color = cv::Scalar(255,255,255); // line color
    cv::circle( image, point, rad, color, -1, 8 );
    
}
    
GenericMap<int, cv::Point*>* BuildCity::getIntersectionsFromMapPNG(cv::Mat map) {
    // find intersection for all roads
    GenericMap<int, cv::Point*>* ints = new GenericMap<int, cv::Point*>();
    
    int pointCount = 1;
    int kernelSideDim = 76; // not dynamically set
    
    for(int row = 0; row < map.rows; row += kernelSideDim)
    {
        std::cout << "row: " << row << std::endl;
        for(int col = 0; col < map.cols; col += kernelSideDim)
        {
            std::cout << "col: " << col << std::endl;
            if(row + kernelSideDim < map.rows && col + kernelSideDim < map.cols)
            {
                cv::Rect roi(row, col, kernelSideDim, kernelSideDim);
                cv::Mat kernel = map(roi);

                int xCount[kernelSideDim];
                int yCount[kernelSideDim];

                std::fill_n(xCount, kernelSideDim, 0);
                std::fill_n(yCount, kernelSideDim, 0);

                std::vector<cv::Vec2f> lines;
                cv::HoughLines(kernel, lines, 1, CV_PI/180, 25, 0, 0 ); // need to dynamically scale point-on-line count according to zoom

                // Draw the hough lines
                std::vector<cv::Vec2f>::const_iterator it1 = lines.begin();
                while(it1 != lines.end())
                {
                    std::pair<cv::Point, cv::Point>* linePoints1 = this->polarToCartisian((*it1)[0], (*it1)[1], kernel.rows);

                    std::vector<cv::Vec2f>::const_iterator it2 = lines.begin();
                    while(it2 != lines.end())
                    {
                        std::pair<cv::Point, cv::Point>* linePoints2 = this->polarToCartisian((*it2)[0], (*it2)[1], kernel.rows);

                        if(std::abs((*it1)[1] - (*it2)[1]) > 10*CV_PI/180) {

                            cv::Point intPnt;
                            bool linesIntersect = getIntersectionPoint(linePoints1->first, linePoints1->second, linePoints2->first, linePoints2->second, intPnt);

                            if(linesIntersect && intPnt.x < kernelSideDim && intPnt.y < kernelSideDim && intPnt.x >= 0 && intPnt.y >= 0)
                            {
                                xCount[intPnt.x] += 1;
                                yCount[intPnt.y] += 1;
                            }
                        }
                        ++it2;
                    }
                    ++it1;
                }

                cv::Point* intersectPnt = new cv::Point();
                intersectPnt->x = getCoord(xCount, kernelSideDim, 2) + row;
                intersectPnt->y = getCoord(yCount, kernelSideDim, 2) + col;
                if(intersectPnt->x != -1)
                {
                    int rad = 4;
                    cv::Scalar color=cv::Scalar(255,255,255); // line color
                    cv::circle(map, *intersectPnt, rad, color, -1, 8);
                    
                    ints->addEntry(pointCount, intersectPnt);
                    pointCount++;
                } else {
                    free(intersectPnt);
                }
            }
        }
    }
    cv::imshow("intersection", map);
    return ints;
}
    
int BuildCity::getCoord(int* dimCount, int dim, int tol) {
    int i, coordCount = 0, max = 0, coord = -1, itr = 0;
    for(i = 0; i < dim; i++) {
        if(itr == tol) {
            if(coordCount > max) {
                max = coordCount;
                coord = i + tol / 2;
            }
            coordCount = 0;
            itr = 0;
        } else if(dimCount[i] != 0) {
            int c = dimCount[i];
            coordCount += c;
        }
        itr++;
    }
    return coord;
}
    
// get intersection points
bool BuildCity::getIntersectionPoint(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2, cv::Point & intPnt) {
    cv::Point p = a1;
    cv::Point q = b1;
    cv::Point r(a2-a1);
    cv::Point s(b2-b1);
    
    if(cross(r,s) == 0) {
        return false;}
    
    double t = cross(q-p,s)/cross(r,s);
    
    intPnt = p + t*r;
    return true;
}
    
double BuildCity::cross(cv::Point v1, cv::Point v2) {
    return v1.x*v2.y - v1.y*v2.x;
}
    
std::pair<cv::Point, cv::Point>* BuildCity::polarToCartisian(float mag, float angle, int rows) {
    cv::Point pt1(mag / cos(angle), 0);
    cv::Point pt2((mag - rows * sin(angle)) / cos(angle), rows);
    return new std::pair<cv::Point, cv::Point>(pt1, pt2);
}

std::pair<int, cv::Mat>* BuildCity::pullAndFormatMapPNG(DataCollection* dc) {
    
    // pull in image
    int zoomIdx = dc->pullDataPNG(this->latCenter, this->lonCenter);
    std::string dataFolder = dc->getDataFolder();
    std::string mapPNGName = dc->getMapPNGName();
    std::string mapPicLoc = dataFolder + "/" + mapPNGName + "-full.png";
    
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
    cv::Scalar yellowLB = cv::Scalar(107, 220, 210);
    cv::Scalar yellowUB = cv::Scalar(205, 255, 255);
    cv::inRange(croppedMapImage, yellowLB, yellowUB, yellowMask);
    
    // white residential road color filter
    cv::Mat whiteMask;
    cv::Scalar whiteLB = cv::Scalar(250, 250, 250);
    cv::Scalar whiteUB = cv::Scalar(255, 255, 255);
    cv::inRange(croppedMapImage, whiteLB, whiteUB, whiteMask);
    
    // orange small highways color filter
    cv::Mat orangeMask;
    cv::Scalar orangeLB = cv::Scalar(120, 180, 220);
    cv::Scalar orangeUB = cv::Scalar(175, 230, 255);
    cv::inRange(croppedMapImage, orangeLB, orangeUB, orangeMask);
    
    cv::imwrite(dataFolder + "/test.png", redMask + yellowMask + whiteMask + orangeMask);
    return new std::pair<int, cv::Mat>(zoomIdx, redMask + yellowMask + whiteMask + orangeMask);
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