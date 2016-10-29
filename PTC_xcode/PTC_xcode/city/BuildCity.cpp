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
    
BuildCity::~BuildCity()
{
    delete(this->rawRoads);
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
        
        // container of raw intersections
        GenericMap<long int, Intersection*> rawInts;;
        
        // get map data
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        this->newBounds = newMapData->second;
        
        // parse new map data to gen raw roads
        dc->pullDataXML(this->latCenter, this->lonCenter);
        this->rawRoads = dc->makeRawRoads();
        
        // gps converter for finding spline ends with close proximity
        GPS converter(this->latCenter, this->lonCenter);
        
        // container of new intersections identified
        this->newInts = new GenericMap<long int, Intersection*>();
        
        // copy of raw roads
        GenericMap<long int, Road*>* rawRoadsCopy = this->rawRoads->copy();
        
        // ******** FIND SPLINE EVALUATIONS IN CLOSE PROXIMITY ********
        this->rawRoads->initializeCounter();
        GenericEntry<long int, Road*>* nextRawRoad = this->rawRoads->nextEntry();
        while(nextRawRoad != NULL)
        {
            std::cout << "***" << nextRawRoad->key << "***" << std::endl;
            
            // spline of current road
            Eigen::Spline<double,2> currSpline = nextRawRoad->value->getSpline();
            float nextEvalStepSize = this->evalIntervalLength / nextRawRoad->value->getSplineLength();
            
            // start at a given evaluation point of the next spline and look for nearest other points in the map
            for(double u = 0; u <= 1.0; u += nextEvalStepSize)
            {
                // create a container of close intersections and store values that are within range
                GenericMap<long int, std::pair<double, double>*> closeSplineEvals;
                
                // get current evaluation point of current spline
                Eigen::Spline<double,2>::PointType currPt = currSpline(u);
                
                // create a count of interesections to given point
                int intersectNum = 0;
                
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
                    
                    // spline of other road
                    Eigen::Spline<double,2> otherSpline = nextOtherRawRoad->value->getSpline();
                    double nextOtherEvalStepSize = this->evalIntervalLength / nextOtherRawRoad->value->getSplineLength();
                
                    // check for close proximity evaluation points
                    for(double v = 0; v <= 1.0; v += nextOtherEvalStepSize)
                    {
                        Eigen::Spline<double,2>::PointType otherPt = otherSpline(v);
                        
                        float evalDist = converter.deltaLatLonToXY(currPt(0,0), currPt(1,0), otherPt(0,0), otherPt(1,0));
                        
                        if(evalDist < this->evalIntervalLength + 2.0)
                        {
                            std::cout << nextOtherRawRoad->key << std::endl;
                            closeSplineEvals.addEntry(nextOtherRawRoad->key, new std::pair<double, double>(otherPt(0,0), otherPt(1,0)));
                        }
                    }
                    
                    nextOtherRawRoad = rawRoadsCopy->nextEntry();
                }
                delete(nextOtherRawRoad);
                
                if(closeSplineEvals.getSize() > 0)
                {
                    double avgLat = currPt(0,0);
                    double avgLon = currPt(1,0);
                    
                    GenericMap<long int, Road*>* connectingRoads = new GenericMap<long int, Road*>();
                    connectingRoads->addEntry(nextRawRoad->key, this->rawRoads->getEntry(nextRawRoad->key));
                    
                    long int intID = nextRawRoad->key;
                    
                    closeSplineEvals.initializeCounter();
                    GenericEntry<long int, std::pair<double, double>*>* nextLatLon = closeSplineEvals.nextEntry();
                    while(nextLatLon != NULL)
                    {
                        avgLat += nextLatLon->value->first;
                        avgLon += nextLatLon->value->second;
                        
                        if(!connectingRoads->hasEntry(nextLatLon->key))
                        {
                            connectingRoads->addEntry(nextLatLon->key, this->rawRoads->getEntry(nextLatLon->key));
                            intID += nextLatLon->key;
                        }
                        
                        delete(nextLatLon->value);
                        nextLatLon = closeSplineEvals.nextEntry();
                    }
                    delete(nextLatLon);
                    
                    if(!rawInts.hasEntry(intID))
                    {
                        float cses = closeSplineEvals.getSize() + 1.0;
                        rawInts.addEntry(intID, new Intersection(connectingRoads, avgLat/cses, avgLon/cses, -1000, intID));
                    }
                        
                }
            }
            nextRawRoad = this->rawRoads->nextEntry();
        }
        delete(nextRawRoad);
        
        // ******** CLUSTER INTERSECTIONS ********
        std::cout << "clustering raw intersections" << std::endl;
        GenericMap<long int, Intersection*> refinedInts;
        
        while(rawInts.getSize() != 0)
        {
            Intersection* rawInt = rawInts.getFirstEntry();
            
            GenericMap<long int, Intersection*> closeInts;
            closeInts.addEntry(rawInt->getIntersectionID(), rawInt);
            
            // iterate through all raw interestcions and find other intersections in close proximity
            rawInts.initializeCounter();
            GenericEntry<long int, Intersection*>* nextRawInt = rawInts.nextEntry();
            while(nextRawInt != NULL)
            {
                // get other intersection
                Intersection* otherRawInt = nextRawInt->value;
                
                // don't look at the same raw intersection
                if(closeInts.hasEntry(otherRawInt->getIntersectionID()))
                {
                    nextRawInt = rawInts.nextEntry();
                    continue;
                }
                
                // iterate through close intersections to see if other raw int is in close proximity to cluster
                closeInts.initializeCounter();
                GenericEntry<long int, Intersection*>* nextCloseInt = closeInts.nextEntry();
                while(nextCloseInt != NULL)
                {
                    float dist = converter.deltaLatLonToXY(nextCloseInt->value->getLat(), nextCloseInt->value->getLon(), otherRawInt->getLat(), otherRawInt->getLon());
                    
                    // tuned for NE greenlake
                    if(dist < 13.0)
                    {
                        // add close proximity intersection to cluster
                        closeInts.addEntry(otherRawInt->getIntersectionID(), otherRawInt);
                        
                        // re-init iterator to assess cluster against whole set of raw ints
                        rawInts.initializeCounter();
                        
                        // break out since intersection was found to be in cluster
                        break;
                    }
                    
                    nextCloseInt = closeInts.nextEntry();
                }
                delete(nextCloseInt);
                
                nextRawInt = rawInts.nextEntry();
            }
            delete(nextRawInt);
            
            // iterate through cluster of close ints and average lat / lon and create list of all connecting roads
            double avgLat = 0;
            double avgLon = 0;
            
            GenericMap<long int, Road*>* connectingRoads = new GenericMap<long int, Road*>();
            int intersectionID = 0;
            
            closeInts.initializeCounter();
            GenericEntry<long int, Intersection*>* nextCloseInt = closeInts.nextEntry();
            while(nextCloseInt != NULL)
            {
                // average lat / lon
                avgLat += nextCloseInt->value->getLat();
                avgLon += nextCloseInt->value->getLon();
                intersectionID += nextCloseInt->value->getIntersectionID();
                
                // add connecting roads to list of all connecting roads
                GenericMap<long int, Road*>* closeIntConnectingRoads = nextCloseInt->value->getRoads();
                
                closeIntConnectingRoads->initializeCounter();
                GenericEntry<long int, Road*>* nextCloseIntConectingRoad = closeIntConnectingRoads->nextEntry();
                while(nextCloseIntConectingRoad != NULL)
                {
                    if(!connectingRoads->hasEntry(nextCloseIntConectingRoad->key))
                    {
                        connectingRoads->addEntry(nextCloseIntConectingRoad->key, nextCloseIntConectingRoad->value);
                    }
                    nextCloseIntConectingRoad = closeIntConnectingRoads->nextEntry();
                }
                delete(nextCloseIntConectingRoad);
                
                // remove close intersections from raw intersections
                rawInts.erase(nextCloseInt->key);
                
                nextCloseInt = closeInts.nextEntry();
            }
            delete(nextCloseInt);
            
            // create new intersection with average lat / lon and new connecting road
            float cis = closeInts.getSize();
            refinedInts.addEntry(intersectionID, new Intersection(connectingRoads, avgLat/cis, avgLon/cis, -1000, intersectionID));
        }
        
        // ******** TRIM ROAD SECTIONS ********

        // pool new raw intersections and existing intersections
        GenericMap<long int, Intersection*> allInts;
        
        if(this->city != NULL && this->city->getIntersections() != NULL && this->city->getIntersections()->getSize() > 0)
        {
            this->city->getIntersections()->initializeCounter(); // existing intersections
            GenericEntry<long int, Intersection*>* nextCityInt = this->city->getIntersections()->nextEntry();
            while(nextCityInt != NULL)
            {
                allInts.addEntry(nextCityInt->key, nextCityInt->value);
                nextCityInt = this->city->getIntersections()->nextEntry();
            }
            delete(nextCityInt);
        }
        
        refinedInts.initializeCounter(); // new intersections
        GenericEntry<long int, Intersection*>* nextRefInt = refinedInts.nextEntry();
        while(nextRefInt != NULL)
        {
            allInts.addEntry(nextRefInt->key, nextRefInt->value);
            nextRefInt = refinedInts.nextEntry();
        }
        delete(nextRefInt);
        
        // iterate through refined ints to make road connections
        refinedInts.initializeCounter();
        GenericEntry<long int, Intersection*>* nextInt = refinedInts.nextEntry();
        while(nextInt != NULL)
        {
            // get connecting roads of intersection
            GenericMap<long int, Road*>* intConnectingRoads = nextInt->value->getRoads();
            
            // create a container of new atomic roads
            GenericMap<long int, Road*>* newConnectingRoads = new GenericMap<long int, Road*>();
            
            // iterate through all connecting roads to find atomic road segments between 2 intersections
            intConnectingRoads->initializeCounter();
            GenericEntry<long int, Road*>* nextConnectingRoad = intConnectingRoads->nextEntry();
            while(nextConnectingRoad != NULL)
            {
                // iterate along connecting road spline to find intersections in close proximity
                Eigen::Spline<double,2> spline = nextConnectingRoad->value->getSpline();
                double evalStepSize = this->evalIntervalLength / nextConnectingRoad->value->getSplineLength();

                // create a container of control points for road
                GenericMap<long int, Node*>* nodes = new GenericMap<long int, Node*>();
                long int splineStartIntID = -1;
                long int splineEndIntID = -1;
                int evalCount = 0;
                
                // iterate along each connecting road until another intersection is found in close proximity
                for(double u = 0; u <= 1.0; u += evalStepSize)
                {
                    Eigen::Spline<double,2>::PointType pt = spline(u);
                    
                    // find where ends of spline meet intersection <---- mix existing intersection in here!
                    allInts.initializeCounter();
                    GenericEntry<long int, Intersection*>* nextOtherInt = allInts.nextEntry();
                    while(nextOtherInt != NULL)
                    {
                        float dist = converter.deltaLatLonToXY(pt(0,0), pt(1,0), nextOtherInt->value->getLat(), nextOtherInt->value->getLon());
                        
                        if(dist < this->evalIntervalLength + 2)
                        {
                            // check to see if intersection already exists on path incase the intersection has 2 close points near spline evaluation pt
                            bool hasInt = false;
                            nodes->initializeCounter();
                            GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
                            while(nextNode != NULL)
                            {
                                if(nextNode->value->getID() == nextOtherInt->value->getIntersectionID())
                                {
                                    hasInt = true;
                                    break;
                                }
                                nextNode = nodes->nextEntry();
                            }
                            delete(nextNode);
                            
                            // break if repeated close int if found near same eval pt
                            if(hasInt) { break; }
                            
                            // add intersection
                            if(splineStartIntID == -1)
                            {
                                splineStartIntID = nextOtherInt->value->getIntersectionID();
                            }
                            
                            else
                            {
                                splineEndIntID = nextOtherInt->value->getIntersectionID();
                            }
                            
                            double intLat = nextOtherInt->value->getLat();
                            double intLon = nextOtherInt->value->getLon();
                            long int intID = nextOtherInt->value->getIntersectionID();
                            
                            nodes->addEntry(evalCount, new Node(intLat, intLon, -1000, intID));
                            evalCount++;
                            
                            break;
                        }
                        
                        nextOtherInt = allInts.nextEntry();
                    }
                    delete(nextOtherInt);
                    
                    // take measurement of spline evaluation between intersections
                    if(splineStartIntID != -1 && splineEndIntID == -1)
                    {
                        nodes->addEntry(evalCount, new Node(pt(0,0), pt(1,0), -1000, nextConnectingRoad->key));
                        evalCount++;
                    }
                    
                    // found road segment between intersection
                    else if (splineStartIntID != -1 && splineEndIntID != -1)
                    {
                        if(splineStartIntID == splineEndIntID)
                        {
                            int test = 2;
                        }
                        
                        // ensure start or end intersection is nextInt
                        bool startIntIsNotNextInt = splineStartIntID != nextInt->key;
                        bool endIntIsNotNextInt = splineEndIntID != nextInt->key;
                        if(startIntIsNotNextInt && endIntIsNotNextInt)
                        {
                            splineStartIntID = -1;
                            splineEndIntID = -1;
                            
                            delete(nodes);
                            nodes = new GenericMap<long int, Node*>();
                            
                            evalCount = 0;
                            continue;
                        }
                        
                        Road* newRoad;
                        
                        // check for unique atomic road segmenet
                        long int adjacentIntID;
                        if(startIntIsNotNextInt)
                        {
                            adjacentIntID = splineStartIntID;
                        }
                        else if(endIntIsNotNextInt)
                        {
                            adjacentIntID = splineEndIntID;
                        }
                        
                        bool foundUniqueRoad = true;
                        GenericMap<long int, Road*>* adjacentIntConnectingRoads = refinedInts.getEntry(adjacentIntID)->getRoads();
                        
                        adjacentIntConnectingRoads->initializeCounter();
                        GenericEntry<long int, Road*>* nextAdjacentIntConnectingRoad = adjacentIntConnectingRoads->nextEntry();
                        while(nextAdjacentIntConnectingRoad != NULL)
                        {
                            Intersection* connectingRoadStartInt = nextAdjacentIntConnectingRoad->value->getStartIntersection();
                            Intersection* connectingRoadEndInt = nextAdjacentIntConnectingRoad->value->getEndIntersection();
                            
                            if(connectingRoadStartInt == NULL || connectingRoadEndInt == NULL)
                            {
                                nextAdjacentIntConnectingRoad = adjacentIntConnectingRoads->nextEntry();
                                continue;
                            }
                            
                            bool hasStartInt = connectingRoadStartInt->getIntersectionID() == splineStartIntID || connectingRoadEndInt->getIntersectionID() == splineStartIntID;
                            bool hasEndInt = connectingRoadStartInt->getIntersectionID() == splineEndIntID || connectingRoadEndInt->getIntersectionID() == splineEndIntID;
                            
                            if(hasStartInt && hasEndInt)
                            {
                                newRoad = nextAdjacentIntConnectingRoad->value;
                                foundUniqueRoad = false;
                                break;
                            }
                            
                            nextAdjacentIntConnectingRoad = adjacentIntConnectingRoads->nextEntry();
                        }
                        delete(nextAdjacentIntConnectingRoad);
                        
                        // create new road with shorted spline if road is unique
                        if(foundUniqueRoad)
                        {
                            // create a container of control points to fit spline to
                            Eigen::MatrixXd points(2, nodes->getSize());
                            int latLonCount = 0;
                            
                            // get first segement evaluation to calculate a distance
                            Node* firstNode = nodes->getFirstEntry();
                            double prevLat = firstNode->getLat();
                            double prevLon = firstNode->getLon();
                            double currLat = 0;
                            double currLon = 0;
                            float dist = 0;
                            
                            // iterate along nodes to fit new shortened spline between interesections
                            nodes->initializeCounter();
                            GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
                            while(nextNode != NULL)
                            {
                                currLat = nextNode->value->getLat();
                                currLon = nextNode->value->getLon();
                                
                                // update spline container
                                points(0, latLonCount) = currLat;
                                points(1, latLonCount) = currLon;
                                latLonCount++;
                                
                                // update distance
                                dist += converter.deltaLatLonToXY(prevLat, prevLon, currLat, currLon);
                                
                                prevLat = currLat;
                                prevLon = currLon;
                                
                                nextNode = nodes->nextEntry();
                            }
                            delete(nextNode);
                            
                            // fit first order spline spline
                            typedef Eigen::Spline<double, 2> spline2f;
                            spline2f atomicRoadSpline = Eigen::SplineFitting<spline2f>::Interpolate(points, 1);
                            
                            // make new road
                            newRoad = new Road(nextConnectingRoad->value->getRoadType(), splineStartIntID + splineEndIntID, nodes);
                            newRoad->assignSpline(atomicRoadSpline);
                            newRoad->assignSplineLength(dist);
                            newRoad->setMinMaxLatLon();
                            newRoad->setStartIntersection(refinedInts.getEntry(splineStartIntID));
                            newRoad->setEndIntersection(refinedInts.getEntry(splineEndIntID));
                            newRoad->setBoundsID(this->newBounds->getID());
                        }
                        
                        newConnectingRoads->addEntry(newRoad->getRoadID(), newRoad);
                        
                        // attempt to grab another connecting road from the same raw road
                        splineStartIntID = -1;
                        splineEndIntID = -1;
                        nodes = new GenericMap<long int, Node*>();
                        evalCount = 0;
                    }
                }
                
                nextConnectingRoad = intConnectingRoads->nextEntry();
            }
            delete(nextConnectingRoad);
            
            nextInt->value->replaceRoads(newConnectingRoads);
            
            this->newInts->addEntry(nextInt->key, nextInt->value);
            nextInt = refinedInts.nextEntry();
        }
        delete(nextInt);
        
        // add elevation
        std::cout << "adding elevations to spline control points" << std::endl;
        this->newInts->initializeCounter();
        GenericEntry<long int, Intersection*>* nextNewInt = this->newInts->nextEntry();
        while(nextNewInt != NULL)
        {
            dc->updateElevationData(nextNewInt->value->getRoads());
            nextNewInt = this->newInts->nextEntry();
        }
        delete(nextNewInt);
    }
}

void BuildCity::printNewIntersectionsAndRoads()
{
    if( this->newBoundsFound && this->newInts->getSize() > 0)
    {
        // csv name
        std::string csvName = "/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/parsedMapData.csv";
        
        // delete existing csv if found
        std::string rm = "rm " + csvName;
        system(rm.c_str());
        
        // create new csv
        FILE* csv;
        csv = std::fopen(csvName.c_str(), "w");
        
        // add header to csv
        fprintf(csv, "name,icon,description,color,latitude,longitude\n");
        
        std::cout << "**** printing intersection lat/lon ****" << std::endl;
        this->newInts->initializeCounter();
        GenericEntry<long int, Intersection*>* nextInt = this->newInts->nextEntry();
        while(nextInt != NULL)
        {
            // print intersection lat/lon to console
            printf("%.12f,%.12f\n", nextInt->value->getLat(), nextInt->value->getLon());
            
            // print intersection lat/lon to csv
            fprintf(csv, "%ld,", nextInt->value->getIntersectionID());
            fprintf(csv, "googlemini,");
            fprintf(csv, "Int ID: %ld | ", nextInt->value->getIntersectionID());
            fprintf(csv, "Int Ele: %f | ", nextInt->value->getElevation());
            fprintf(csv, "Lat & Lon: %.12f %.12f | ", nextInt->value->getLat(), nextInt->value->getLon());
            fprintf(csv, "Connecting Roads: ");
            
            nextInt->value->getRoads()->initializeCounter();
            GenericEntry<long int, Road*>* nextConnectingRoad = nextInt->value->getRoads()->nextEntry();
            while(nextConnectingRoad != NULL)
            {
                fprintf(csv, "%ld ", nextConnectingRoad->key);
                nextConnectingRoad = nextInt->value->getRoads()->nextEntry();
            }
            delete(nextConnectingRoad);
            
            fprintf(csv, ",");
            fprintf(csv, "red,");
            fprintf(csv, "%.12f,%.12f\n", nextInt->value->getLat(), nextInt->value->getLon());
            
            // print road spline to csv
            nextInt->value->getRoads()->initializeCounter();
            nextConnectingRoad = nextInt->value->getRoads()->nextEntry();
            while(nextConnectingRoad != NULL)
            {
                // iterate along connecting road spline control points
                nextConnectingRoad->value->getNodes()->initializeCounter();
                GenericEntry<long int, Node*>* nextNode = nextConnectingRoad->value->getNodes()->nextEntry();
                
                // burn a node so they are not superimposed on intersection
                nextNode = nextConnectingRoad->value->getNodes()->nextEntry();
                
                int iterCount = 1;
                while(iterCount < nextConnectingRoad->value->getNodes()->getSize() - 1)
                {
                    fprintf(csv, "%ld,", nextConnectingRoad->value->getRoadID());
                    fprintf(csv, "cirlce,");
                    fprintf(csv, "Road ID: %ld | ", nextConnectingRoad->value->getRoadID());
                    fprintf(csv, "Start Int ID: %ld | ", nextConnectingRoad->value->getStartIntersection()->getIntersectionID());
                    fprintf(csv, "Node Ele: %f | ", nextNode->value->getEle());
                    fprintf(csv, "End Int ID: %ld | ", nextConnectingRoad->value->getEndIntersection()->getIntersectionID());
                    fprintf(csv, "Lat & Lon: %.12f %.12f,", nextNode->value->getLat(), nextNode->value->getLon());
                    fprintf(csv, "blue,");
                    fprintf(csv, "%.12f,%.12f\n", nextNode->value->getLat(), nextNode->value->getLon());
                    
                    nextNode = nextConnectingRoad->value->getNodes()->nextEntry();
                    iterCount++;
                }
                delete(nextNode);
                
                nextConnectingRoad = nextInt->value->getRoads()->nextEntry();
            }
            delete(nextConnectingRoad);
            
            nextInt = this->newInts->nextEntry();
        }
        delete(nextInt);
        fclose(csv);
    }
}

void BuildCity::updateGridDataXMLAdj() {
    
    if(this->hasNewBounds())
    {
        std::cout << "identifying intersections from XML Adjacency Matrix" << std::endl;
        std::pair<DataCollection*, Bounds*>* newMapData = this->setupDataCollection();
        DataCollection* dc = newMapData->first;
        this->newBounds = newMapData->second;
        
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
                delete(nextNode);
                
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
        delete(currIdx);
        delete(nextIdx);
        
        nextRawRoad = this->rawRoads->nextEntry();
    }
    delete(nextRawRoad);
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
        this->newBounds = newMapData->second;
        
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
                        delete(intersectPnt);
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
    this->city = dm.getCityData();
    
    this->maxLat = -DBL_MAX;
    this->maxLon = -DBL_MAX;
    this->minLat = DBL_MAX;
    this->minLon = DBL_MAX;
    
    tripLatLon->initializeCounter();
    GenericEntry<long int, std::pair<double, double>*>* nextTripLatLon = tripLatLon->nextEntry();
    if(this->city != NULL)
    {
        // bounds data already exists
        GenericMap<int, Bounds*>* bounds = this->city->getBoundsMap();
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
                    this->newBoundsFound = true;
                    if(lat > this->maxLat) { this->maxLat = lat; } if(lat < this->minLat) { this->minLat = lat; }
                    if(lon > this->maxLon) { this->maxLon = lon; } if(lon < this->minLon) { this->minLon = lon; }
                    
                }
                nextBounds = bounds->nextEntry();
            }
            nextTripLatLon = tripLatLon->nextEntry();
            delete(nextBounds);
        }
    } else {
        // no bounds data
        this->newBoundsFound = true;
        while(nextTripLatLon != NULL)
        {
            double lat = nextTripLatLon->value->first;
            double lon = nextTripLatLon->value->second;
            
            if(lat > this->maxLat) { this->maxLat = lat; } if(lat < this->minLat) { this->minLat = lat; }
            if(lon > this->maxLon) { this->maxLon = lon; } if(lon < this->minLon) { this->minLon = lon; }
            nextTripLatLon = tripLatLon->nextEntry();
        }
    }
    delete(nextTripLatLon);
    
    return this->newBoundsFound;
}
    
GenericMap<long int, Intersection*>* BuildCity::getNewIntersections()
{
    return this->newInts;
}
    
GenericMap<long int, Road*>* BuildCity::getNewRoads()
{
    GenericMap<long int, Road*>* newRoads = new GenericMap<long int, Road*>();
    
    this->newInts->initializeCounter();
    GenericEntry<long int, Intersection*>* nextInt = this->newInts->nextEntry();
    while(nextInt != NULL)
    {
        GenericMap<long int, Road*>* connectingRoads = nextInt->value->getRoads();
        
        connectingRoads->initializeCounter();
        GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
        while(nextRoad != NULL)
        {
            if(!newRoads->hasEntry(nextRoad->value->getRoadID()))
            {
                newRoads->addEntry(nextRoad->value->getRoadID(), nextRoad->value);
            }
            nextRoad = connectingRoads->nextEntry();
        }
        delete(nextRoad);
        
        nextInt = this->newInts->nextEntry();
    }
    delete(nextInt);
    
    return newRoads;
}
    
Bounds* BuildCity::getNewBounds()
{
    return this->newBounds;
}
    
} // end of predictivepowertrain
