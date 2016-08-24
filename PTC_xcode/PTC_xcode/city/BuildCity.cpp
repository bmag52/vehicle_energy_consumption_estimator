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
    
void BuildCity::updateGridDataXMLSpline()
{
    if(this->hasNewBounds())
    {
        std::cout << "identifying intersections from XML splines" << std::endl;
        
        // get map data
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        Bounds* newBounds = newMapData->second;
        
        // parse new map data to gen raw roads
        dc->pullDataXML(this->latCenter, this->lonCenter);
        this->rawRoads = dc->makeRawRoads();
        
        // iterate through each road and check every other road for intersection
        int intCount = 1;
        this->rawRoads->initializeCounter();
        GenericEntry<long int, Road*>* nextRawRoad = this->rawRoads->nextEntry();
        GPS converter(this->latCenter, this->lonCenter);
        
        this->newInts = new GenericMap<long int, Intersection*>();
        GenericMap<long int, Road*>* rawRoadsCopy = this->rawRoads->copy();
        while(nextRawRoad != NULL)
        {
            // spline of current road
            Eigen::Spline<double,2> currSpline = nextRawRoad->value->getSpline();
            
            // loop through all other roads
            rawRoadsCopy->initializeCounter();
            GenericEntry<long int, Road*>* nextOtherRawRoad = rawRoadsCopy->nextEntry();
            while(nextOtherRawRoad != NULL)
            {
                // don't look at the same roads
                if(nextRawRoad->key == nextOtherRawRoad->key)
                {
                    nextOtherRawRoad = rawRoadsCopy->nextEntry();
                    continue;
                }
                
                // count number of times the roads intersect.
                int roadPairIntCount = 0;
                
                // get other road spline
                Eigen::Spline<double,2> nextSpline = nextOtherRawRoad->value->getSpline();
                
                // iteratate through road splines
                Eigen::Spline<double,2>::PointType currPtA = currSpline(0);
                std::pair<double, double>* currA_xy = converter.convertLatLonToXY(currPtA(0,0), currPtA(1,0));
                for(double u = this->splineStep; u <= 1; u += this->splineStep)
                {
                    Eigen::Spline<double,2>::PointType currPtB = currSpline(u);
                    std::pair<double, double>* currB_xy = converter.convertLatLonToXY(currPtB(0,0), currPtB(1,0));
                    
                    // iterate through other spline
                    Eigen::Spline<double,2>::PointType nextPtA = nextSpline(0);
                    std::pair<double, double>* nextA_xy = converter.convertLatLonToXY(nextPtA(0,0), nextPtA(1,0));
                    for(double s = this->splineStep; s <= 1; s += this->splineStep)
                    {
                        Eigen::Spline<double,2>::PointType nextPtB = nextSpline(s);
                        std::pair<double, double>* nextB_xy = converter.convertLatLonToXY(nextPtB(0,0), nextPtB(1,0));

                        
//                        if(nextRawRoad->key == 6438214 && nextOtherRawRoad->key == 35171644 && u > .15 && s > .2)
//                        {
//                            int test = 2;
//                        }
                        
                        cv::Point_<double> currA(currA_xy->first, currA_xy->second);
                        cv::Point_<double> currB(currB_xy->first, currB_xy->second);
                        cv::Point_<double> nextA(nextA_xy->first, nextA_xy->second);
                        cv::Point_<double> nextB(nextB_xy->first, nextB_xy->second);
                        cv::Point_<double> intersect;
                        
                        // get intersection and expect roads to only intersect once (take for int)
                        if(this->getIntersectionPoint(currA, currB, nextA, nextB, intersect) && roadPairIntCount < 1)
                        {
                            GenericMap<long int, Road*>* intRoads = new GenericMap<long int, Road*>();
                            intRoads->addEntry(nextRawRoad->key, nextRawRoad->value);
                            intRoads->addEntry(nextOtherRawRoad->key, nextOtherRawRoad->value);
                            
                            std::pair<double, double>* latLon = converter.convertXYToLatLon(intersect.x, intersect.y);
                            Intersection* newInt = new Intersection(intRoads, latLon->first, latLon->second, 0, 0);
                            this->newInts->addEntry(intCount, newInt);
                            
                            free(latLon);
                            roadPairIntCount++;
                            intCount++;
                        }
                        free(nextA_xy);
                        nextA_xy = nextB_xy;
                    }
                    free(currA_xy);
                    currA_xy = currB_xy;
                }
                nextOtherRawRoad = rawRoadsCopy->nextEntry();
            }
            free(nextOtherRawRoad);
            nextRawRoad = this->rawRoads->nextEntry();
        }
        free(nextRawRoad);
    }
}

void BuildCity::printIntersections()
{
    if(this->newInts->getSize() > 0)
    {
        std::cout << "************ printing intersection lat/lon ***********" << std::endl;
        this->newInts->initializeCounter();
        GenericEntry<long int, Intersection*>* nextInt = this->newInts->nextEntry();
        while(nextInt != NULL)
        {
            printf("%.12f,%.12f\n", nextInt->value->getLat(), nextInt->value->getLon());
            nextInt = this->newInts->nextEntry();
        }
        free(nextInt);
    }
}

void BuildCity::updateGridDataXMLAdj() {
    
    if(this->hasNewBounds())
    {
        std::cout << "identifying intersections from XML Adjacency Matrix" << std::endl;
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        Bounds* newBounds = newMapData->second;
        
        dc->pullDataXML(this->latCenter, this->lonCenter);
        this->rawRoads = dc->makeRawRoads();
        
        int latRowsSpline = this->latDelta/this->adjMatPrecFromSplines;
        int lonColsSpline = this->lonDelta/this->adjMatPrecFromSplines;
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
        std::string csvName = "/Users/Brian/Desktop/ecocar/Git/predictive_thermo_controller/data/   adjMatSpline.csv";
        
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
    
    return std::abs(x1-x2) <= 1 && std::abs(y1-y2) <= 1;
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
        
        GenericMap<int, cv::Point*>* rawInts = getIntersectionPointsFromMapPNG(map, zoomIdx);
        
        std::pair<GenericMap<int, Road*>*, GenericMap<int, Intersection*>*>* roadsAndInts = makeRoadsAndIntersections(rawInts, map);

        // Display the detected hough lines
        cv::imshow("incoming road dots", map);
        cv::imwrite("/Users/Brian/Desktop/misc/kernelDev.png", map);
    }
}
    
std::pair<GenericMap<int, Road*>*, GenericMap<int, Intersection*>*>* BuildCity::makeRoadsAndIntersections(GenericMap<int, cv::Point*>* rawInts, cv::Mat map) {
 
    GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
    GenericMap<int, Road*>* roads = new GenericMap<int, Road*>();
    int searchKernelSideDim = 700;
    
    rawInts->initializeCounter();
    cv::Point* nextCentralIntPoint = rawInts->getFirstEntry();
    while(nextCentralIntPoint != NULL)
    {
        
        // get find all intersection points in close proximity to current intersection
        int startRow = nextCentralIntPoint->x - .5*searchKernelSideDim;
        int endRow = nextCentralIntPoint->x + .5*searchKernelSideDim;
        int startCol = nextCentralIntPoint->y -.5*searchKernelSideDim;
        int endCol = nextCentralIntPoint->y + .5*searchKernelSideDim;
        
        GenericMap<int, double>* closeIntPnts = new GenericMap<int, double>();
        
        for(int row = startRow; row < endRow; row++)
        {
            for(int col = startCol; col < endCol; col++)
            {
                int key = hashCoords(col, row);
                if(rawInts->hasEntry(key))
                {
                    double distance = sqrt(pow(nextCentralIntPoint->x - col, 2) + pow(nextCentralIntPoint->y - row, 2));
                    closeIntPnts->addEntry(key, distance);
                }
            }
        }
        
        GenericMap<int, cv::Point*> searchedIntPnts;
        
        while(true)
        {
            GenericEntry<int, double>* nextClosestIntPntData = closeIntPnts->getMinEntry();
            cv::Point* nextClosestIntPnt = rawInts->getEntry(nextClosestIntPntData->key);
            
            // trace out to closest point
            std::pair<bool, GenericMap<int, cv::Point*>*>* roadTraceData = traceRoad(nextClosestIntPnt, nextCentralIntPoint, map, searchedIntPnts);

        }
        
        nextCentralIntPoint = rawInts->getFirstEntry();
    }
    
    
    return NULL;
}
    
std::pair<bool, GenericMap<int, cv::Point*>*>* BuildCity::traceRoad(cv::Point* nextClosestPnt, cv::Point* pnt, cv::Mat map, GenericMap<int, cv::Point*>& searchedIntPnts) {
    
    bool isPreTraveledRoad = false;
    GenericMap<int, cv::Point*>* roadPntTrace = new GenericMap<int, cv::Point*>();
    
    for(int deg = 0; deg < 360; deg++)
    {
        
    }
    
    return new std::pair<bool, GenericMap<int, cv::Point*>*>;
}
    
GenericMap<int, std::pair<cv::Point*, cv::Point*>*>* BuildCity::perimeterScanKernelForRoads(cv::Mat kernel) {
    
    GenericMap<int, cv::Point*> points;
    
    int lastPixel = kernel.at<uchar>(0, 0);
    
    // top edge
    std::cout << "top edge" << std::endl;
    for(int i = 1; i < kernel.cols; i++)
    {
        checkNextPixel(i, 0, points, kernel, lastPixel);
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
    }
    lastPixel = nextPixel;
}
    
// Draw the detected intersection point on an image
void BuildCity::drawPoint(cv::Mat &image, cv::Point point) {
    int rad = 7;
    cv::Scalar color = cv::Scalar(255,255,255); // line color
    cv::circle( image, point, rad, color, -1, 8 );
}
    
BuildCity::zoomParams BuildCity::getZoomParams(int zoom) {
    
    zoomParams newZoomParams;
    
    switch(zoom)
    {
        case 19 :
            newZoomParams.kernelSideDim = 175;
            newZoomParams.coordBinResolution = 6;
            newZoomParams.maxLines = 200;
            newZoomParams.maxClusterPntDistance = 25;
            break;
        case 18 :
            newZoomParams.kernelSideDim = 150;
            newZoomParams.coordBinResolution = 6;
            newZoomParams.maxLines = 200;
            newZoomParams.maxClusterPntDistance = 20;
            break;
        default :
            std::cout << "no zoom params for " << zoom << std::endl;
    }
    
    newZoomParams.angleThreshold = 30;
    newZoomParams.pntThreshold = .75*newZoomParams.kernelSideDim;
    newZoomParams.imageProcessingResolution = .20*newZoomParams.kernelSideDim;
    
    return newZoomParams;
}
    
GenericMap<int, cv::Point*>* BuildCity::getIntersectionPointsFromMapPNG(cv::Mat map, int zoom) {

    GenericMap<int, cv::Point*> rawIntersectionPoints;
    
    zoomParams zp = getZoomParams(zoom);
    
    for(int row = 0; row < map.rows; row += zp.imageProcessingResolution)
    {
        for(int col = 0; col < map.cols; col += zp.imageProcessingResolution)
        {
            if((row + zp.kernelSideDim) < map.rows && (col + zp.kernelSideDim) < map.cols)
            {
                cv::Rect roi(col, row, zp.kernelSideDim, zp.kernelSideDim);
                cv::Mat kernel = map(roi);

                std::vector<cv::Vec2f> lines;
                cv::HoughLines(kernel, lines, 1, CV_PI/180, zp.pntThreshold, 0, 0 );

                // Draw the hough lines
                std::vector<cv::Vec2f>::const_iterator it1 = lines.begin();
                if(lines.size() < zp.maxLines)
                {
                    int xCount[zp.kernelSideDim];
                    int yCount[zp.kernelSideDim];
                    
                    std::fill_n(xCount, zp.kernelSideDim, 0);
                    std::fill_n(yCount, zp.kernelSideDim, 0);
                    
                    while(it1 != lines.end())
                    {
                        std::pair<cv::Point, cv::Point>* linePoints1 = this->polarToCartisian((*it1)[0], (*it1)[1], kernel.rows);

                        std::vector<cv::Vec2f>::const_iterator it2 = lines.begin();
                        while(it2 != lines.end())
                        {
                            std::pair<cv::Point, cv::Point>* linePoints2 = this->polarToCartisian((*it2)[0], (*it2)[1], kernel.rows);

                            double lineAngle = std::abs((*it1)[1] - (*it2)[1])*180/CV_PI;
                            if(lineAngle > zp.angleThreshold && lineAngle < 180 - zp.angleThreshold) {

                                cv::Point intPnt;
                                bool linesIntersect = getIntersectionPoint(linePoints1->first, linePoints1->second, linePoints2->first, linePoints2->second, intPnt);

                                if(linesIntersect && intPnt.x < zp.kernelSideDim && intPnt.y < zp.kernelSideDim && intPnt.x >= 0 && intPnt.y >= 0)
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
                    intersectPnt->x = getCoord(xCount, zp.kernelSideDim, zp.coordBinResolution);
                    intersectPnt->y = getCoord(yCount, zp.kernelSideDim, zp.coordBinResolution);
                    
                    if(intersectPnt->x != -1)
                    {
                        intersectPnt->x += col;
                        intersectPnt->y += row;

                        rawIntersectionPoints.addEntry(hashCoords(intersectPnt->x, intersectPnt->y), intersectPnt);
                        
                    } else {
                        free(intersectPnt);
                    }
                }
            }
        }
    }
    
    // pick best point from cluster
    GenericMap<int, cv::Point*>* intersections = new GenericMap<int, cv::Point*>();
    
    cv::Point* nextRawIntersectionPoint = rawIntersectionPoints.getFirstEntry();
    while(nextRawIntersectionPoint != NULL)
    {
        GenericMap<int, cv::Point*> intersectionPointCluster;
        
        int key = hashCoords(nextRawIntersectionPoint->x, nextRawIntersectionPoint->y);
        intersectionPointCluster.addEntry(key, nextRawIntersectionPoint);
        rawIntersectionPoints.erase(key);
        
        declusterIntersectionPoints(nextRawIntersectionPoint, rawIntersectionPoints, intersectionPointCluster, .25*zp.kernelSideDim, zp.maxClusterPntDistance);
        
        // average point locations in cluster and plop new intersection point
        if(intersectionPointCluster.getSize() > 1)
        {
            int sumX = 0;
            int sumY = 0;
            
            intersectionPointCluster.initializeCounter();
            GenericEntry<int, cv::Point*>* nextIntPnt = intersectionPointCluster.nextEntry();
            while(nextIntPnt != NULL)
            {
                sumX += nextIntPnt->value->x;
                sumY += nextIntPnt->value->y;
                
                nextIntPnt = intersectionPointCluster.nextEntry();
            }
            
            int avgX = sumX / intersectionPointCluster.getSize();
            int avgY = sumY / intersectionPointCluster.getSize();
            
            cv::Point* intersection = new cv::Point(avgX, avgY);
            intersections->addEntry(hashCoords(avgX, avgY), intersection);
            
            cv::circle(map, *intersection, 3, cv::Scalar(0,0,0));
        }
        nextRawIntersectionPoint = rawIntersectionPoints.getFirstEntry();
    }
    
    return intersections;
}
    
void BuildCity::declusterIntersectionPoints(cv::Point* rawIntPnt, GenericMap<int, cv::Point*>& rawIntPnts, GenericMap<int, cv::Point*>& intPnts, int kernelSideDim, double maxDistance)
{
    int rawIntPntX = rawIntPnt->x;
    int rawIntPntY = rawIntPnt->y;
    
    int rowStart = rawIntPntY - .5*kernelSideDim;
    int rowEnd = rawIntPntY + .5*kernelSideDim;
    int colStart = rawIntPntX - .5*kernelSideDim;
    int colEnd = rawIntPntX + .5*kernelSideDim;
    
    for(int row = rowStart; row < rowEnd; row++)
    {
        for(int col = colStart; col < colEnd; col++)
        {
            int key = hashCoords(col, row);
            if(rawIntPnts.hasEntry(key))
            {
                double distance = sqrt(pow(rawIntPntX - col, 2) + pow(rawIntPntY - row, 2));
                if(distance < maxDistance)
                {
                    cv::Point* closeRawIntPnt = rawIntPnts.getEntry(key);
                   
                    rawIntPnts.erase(key);
                    intPnts.addEntry(key, closeRawIntPnt);
                    declusterIntersectionPoints(closeRawIntPnt, rawIntPnts, intPnts, kernelSideDim, maxDistance);
                }
            }
        }
    }
}
    
int BuildCity::hashCoords(int x, int y) {
    return y * 6000 + x; // assign pixel number counting left to right, top to bottom
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
template<typename K>
bool BuildCity::getIntersectionPoint(cv::Point_<K> a1, cv::Point_<K> a2, cv::Point_<K> b1, cv::Point_<K> b2, cv::Point_<K> & intPnt) {
    cv::Point_<K> p = a1;
    cv::Point_<K> q = b1;
    cv::Point_<K> r(a2-a1);
    cv::Point_<K> s(b2-b1);
    
    // check for crossage
    double crossage = cross(r,s);
    if(crossage < .00000001)
    {
        return false;
    }
    
    double t = cross(q-p,s)/cross(r,s);
    
    intPnt = p + t*r;
    return true;
}

template<typename K>
double BuildCity::cross(cv::Point_<K> v1, cv::Point_<K> v2) {
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