/*
 * CityObj.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: Amanda
 */

#include "City.h"

namespace PredictivePowertrain {

City::City() {
    this->roads = new GenericMap<long int, Road*>();
    this->intersections = new GenericMap<long int, Intersection*>();
    this->boundsMap = new GenericMap<int, Bounds*>();
    std::srand(std::time(0));
}

City::City(GenericMap<long int, Intersection*>* intersections, GenericMap<long int, Road*>* roads, GenericMap<int, Bounds*>* boundsMap) {
	this->roads = roads;
	this->intersections = intersections;
	this->boundsMap = boundsMap;
    std::srand(std::time(0));
}
    
City::~City()
{
    delete(this->roads);
    delete(this->intersections);
    delete(this->boundsMap);
}

int City::getRoadMapSize() {
	return this->roads->getSize();
}

int City::getInstersectionMapSize() {
	return this->intersections->getSize();
}

GenericMap<long int, Link*>* City::getNextLinks(Link* otherLink) {
	assert(!otherLink->isFinalLink());
	Road* currentRoad = this->roads->getEntry(otherLink->getNumber());
	Intersection* nextIntersection = getIntersectionFromLink(otherLink, true);

	GenericMap<long int, Link*>* nextLinks = new GenericMap<long int, Link*>();
	GenericMap<long int, Road*>* connectingRoads = nextIntersection->getRoads();

	long int count = 0;
	connectingRoads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
	while(nextRoad != NULL)
	{
		if(nextRoad->value->getRoadID() != currentRoad->getRoadID())
		{
			Link* newLink = otherLink->linkFromRoad(nextRoad->value, nextIntersection);
			nextLinks->addEntry(count, newLink);
            count++;
		}
		nextRoad = connectingRoads->nextEntry();
	}
    delete(nextRoad);
	nextLinks->addEntry(count, this->link->finalLink());
	return nextLinks;
}

Intersection* City::getIntersectionFromLink(Link* link, bool isIntersection) {
	assert(!link->isFinalLink());
	Road* road = this->roads->getEntry(link->getNumber());

	if(!(link->getDirection() == 0) != !isIntersection)
	{
		return road->getStartIntersection();
	} else {
		return road->getEndIntersection();
	}
}

Intersection* City::getIntersection(long int intersectionNum) {
	assert(getInstersectionMapSize() < intersectionNum || intersectionNum < 1); // || this->intersections[intersectionNum] == NULL
	return this->intersections->getEntry(intersectionNum);
}

Route* City::getPath(Intersection* start, Intersection* end, std::vector<float>* conditions, int fastest)
{
	GenericMap<long int, float> dist;
	GenericMap<long int, long int> prev;
	GenericMap<long int, float> unvisitedNodes;

	GenericMap<long int, Intersection*>* adjInts = start->getAdjacentIntersections();

	adjInts->initializeCounter();
	GenericEntry<long int, Intersection*>* nextIntersection = adjInts->nextEntry();
	while(nextIntersection != NULL)
	{
		long int intNum = nextIntersection->value->getIntersectionID();
		dist.addEntry(intNum, FLT_MAX);
		prev.addEntry(intNum, -1);
		nextIntersection = adjInts->nextEntry();
	}
    delete(nextIntersection);
    
    dist.addEntry(start->getIntersectionID(), 0);

	long int closestIntNum = start->getIntersectionID();
	long int distance = 0;
	while(distance != FLT_MAX)
	{
		unvisitedNodes.addEntry(closestIntNum, FLT_MAX);
		GenericMap<long int, Intersection*>* neighbors = getIntersection(closestIntNum)->getAdjacentIntersections();
        
		neighbors->initializeCounter();
		GenericEntry<long int, Intersection*>* nextNeighbor = neighbors->nextEntry();
		while(nextNeighbor != NULL)
		{
			long int neighborNum = nextNeighbor->value->getIntersectionID();
            
            Road* connectingRoad = getConnectingRoad(getIntersection(closestIntNum), nextNeighbor->value);
            
			float alt = dist.getEntry(neighborNum) + connectingRoad->getSplineLength();
			if(dist.getEntry(neighborNum) > alt || !unvisitedNodes.hasEntry(neighborNum))
			{
				dist.addEntry(neighborNum, alt);
				prev.addEntry(neighborNum, closestIntNum);
				unvisitedNodes.addEntry(neighborNum, alt);
			}
			nextNeighbor = neighbors->nextEntry();
		}

		GenericEntry<long int, float>* closestIntPair = unvisitedNodes.getMinEntry();
		distance = closestIntPair->value;
		closestIntNum = closestIntPair->key;
	}

	int linkCount = 0;
	GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
	long int currentIntNum = end->getIntersectionID();
	while(currentIntNum != start->getIntersectionID())
	{
		Intersection* previousInt = getIntersection(prev.getEntry(currentIntNum));
		links->addEntry(linkCount++, this->link->linkFromRoad(getConnectingRoad(previousInt, getIntersection(currentIntNum)), previousInt));
		currentIntNum = previousInt->getIntersectionID();
	}

	links->addEntry(linkCount++, this->link->finalLink());
	Goal* goal = new Goal(end->getIntersectionID(), conditions);
	Route* route = new Route(links, goal);
	return route;
}

std::pair<std::vector<float>, std::vector<float>> City::getData(Road* road, int direction, SpeedPrediction* sp, Eigen::MatrixXd* spdIn, float distAlongRoad)
{
    // get elevation Data
    std::vector<float> elevData;
    std::vector<float> elevDistData;
    road->getElevData(&elevData, &elevDistData);
    
    if(direction)
    {
        for(int i = 0; i < elevData.size(); i++)
        {
            elevData.at(i) = elevData.at(elevData.size() - i - 1);
        }
    }
    assert(elevDistData.size() > 2);
    int prevElevMeasIdx = 0;

    // set output values
    std::vector<float> spdOut;
    std::vector<float> elevDataInterp;
    
    // iterate along speed trace to interpolate elevation trace
    float spdDist = 0.0;
    bool spdDistExceedsRoadDist = false;
    
    Eigen::MatrixXd spdOut_i(1, sp->getO());
    Eigen::MatrixXd spdIn_i(*spdIn);
    
    // format speed input
    sp->formatInData(&spdIn_i);
    
    while(!spdDistExceedsRoadDist)
    {
        // predict speed
        sp->predict(&spdIn_i, &spdOut_i);
        
        // iterate through spd pred output. skip first val since first val is curr spd
        for(int i = 1; i < spdOut_i.cols(); i++)
        {
            spdDist += sp->getDS();
            
            if(spdDist >= distAlongRoad && spdDist <= road->getSplineLength() + sp->getDS())
            {
                // interpolate elevation data
                for(int j = prevElevMeasIdx; j < elevDistData.size() - 1; j++)
                {
                    if(spdDist > elevDistData.at(j))
                    {
                        prevElevMeasIdx = j;
                    }
                    else
                    {
                        break;
                    }
                }
                
                // get delta in elevation from previous to next elevation measurement
                float prev2NextElevDelta = elevData.at(prevElevMeasIdx) - elevData.at(prevElevMeasIdx + 1);
                
                // get delta in distnce from previous to next elevation measurement
                float next2PrevElevDist = elevDistData.at(prevElevMeasIdx + 1) - elevDistData.at(prevElevMeasIdx);
                
                // get delta in distance from previous elevation measurement to current speed distance
                float prevElev2SpdDist = spdDist - elevDistData.at(prevElevMeasIdx);
                
                // derive interpolation factor as a function prev 2 next elevation meas and speed distance after prev elev meas
                float interpFactor = prevElev2SpdDist / next2PrevElevDist;
                
                spdOut.push_back((spdOut_i.coeffRef(0, i) - sp->getSpeedOffset()) * sp->getMaxSpeed());
                elevDataInterp.push_back(elevData.at(prevElevMeasIdx) + interpFactor * prev2NextElevDelta);
            }
            else if(spdDist > road->getSplineLength())
            {
                spdDistExceedsRoadDist = true;
                break;
            }
        }
        
        if(!spdDistExceedsRoadDist)
        {
            // place output into input and repeat if needed
            sp->output2Input(&spdIn_i, &spdOut_i);
        }
    }
    
    return std::pair<std::vector<float>, std::vector<float>>(spdOut, elevDataInterp);
}

Road* City::getConnectingRoad(Intersection* one, Intersection* two) {
	GenericMap<long int, Road*>* roads = one->getRoads();
	roads->initializeCounter();
	GenericEntry<long int, Road*>* nextRoad = roads->nextEntry();
	while(nextRoad != NULL)
	{
		if(one->getNextIntersection(nextRoad->value)->getIntersectionID() == two->getIntersectionID())
		{
			return nextRoad->value;
		}
		nextRoad = roads->nextEntry();
	}
	return NULL;
}

Route* City::randomPath(Intersection* startInt, Route* initialRoute, int totalLength, std::vector<float>* conditions) {
    
	GenericMap<long int, Link*>* startLinks = startInt->getOutgoingLinks();
    GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
    
    // copy outgoing links to prevent mutilation
    startLinks->initializeCounter();
    GenericEntry<long int, Link*>* nextLinkEntry = startLinks->nextEntry();
    while(nextLinkEntry != NULL)
    {
        Link* outLink = nextLinkEntry->value;
        links->addEntry(nextLinkEntry->key, new Link(outLink->getDirection(), outLink->getNumber()));
        nextLinkEntry = startLinks->nextEntry();
    }
    delete(nextLinkEntry);
    
	GenericMap<long int, Link*>* path = new GenericMap<long int, Link*>();
	GenericMap<long int, Intersection*> passedInts;
	passedInts.addEntry(startInt->getIntersectionID(), startInt);
    
	for(int i = 0; i < totalLength; i++)
	{
		Link* nextLink = NULL;
		if(initialRoute->getLinkSize() > i)
		{
			links->initializeCounter();
			GenericEntry<long int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(nextLinkEntry->value->isEqual(initialRoute->getLinks()->getEntry(i)))
				{
					nextLink = initialRoute->getLinks()->getEntry(i);
					break;
				}
				nextLinkEntry = links->nextEntry();
			}
            delete(nextLinkEntry);
		}
        
		if(nextLink == NULL)
		{
			links->initializeCounter();
			GenericEntry<long int, Link*>* nextLinkEntry = links->nextEntry();
			while(nextLinkEntry != NULL)
			{
				if(!nextLinkEntry->value->isFinalLink())
				{
					Intersection* intersection = getIntersectionFromLink(nextLinkEntry->value, true);

					passedInts.initializeCounter();
					GenericEntry<long int, Intersection*>* nextPassedInt = passedInts.nextEntry();
					while(nextPassedInt != NULL)
					{
						if(intersection->getIntersectionID() == nextPassedInt->value->getIntersectionID())
						{
							links->indexErase(nextLinkEntry->key);
						}
						nextPassedInt = passedInts.nextEntry();
					}
                    delete(nextPassedInt);
				}
                nextLinkEntry = links->nextEntry();
            }
            delete(nextLinkEntry);
            
            // break of no next link
            if(links->getSize() == 1) { break; }
            
            int randIdx = std::floor((float)std::rand() / RAND_MAX * links->getSize());
            nextLink = links->getEntry(randIdx);
		}
        
		while(nextLink->isFinalLink() && links->getSize() > 1 && nextLink != NULL)
		{
            int randIdx = std::floor((float)std::rand() / RAND_MAX * links->getSize());
			nextLink = links->getEntry(randIdx);
		}
        
		Intersection* newPassedInt = getIntersectionFromLink(nextLink, true);
		passedInts.addEntry(newPassedInt->getIntersectionID(), newPassedInt);
		path->addEntry(i, nextLink);
        
        delete(links);
		links = getNextLinks(nextLink);
	}
	path->addEntry(path->getSize(), this->link->finalLink());
	Goal* goal = new Goal(getIntersectionFromLink(path->getEntry(path->getSize()-2),true)->getIntersectionID(), conditions);
	Route *route = new Route(path, goal);
    
	return route;
}

bool City::legalRoute(Route* route) {
	Route* routeCopy = route->copy();
	while(routeCopy->getLinkSize() > 1)
	{
		GenericMap<long int, Link*>* legalLinks = getNextLinks(routeCopy->getLinks()->getEntry(1));
		legalLinks->initializeCounter();
		GenericEntry<long int, Link*>* nextLink = legalLinks->nextEntry();
		bool error = true;
		while(nextLink != NULL)
		{
			if(nextLink->value->isEqual(routeCopy->getLinks()->getEntry(1)))
			{
				error = false;
				break;
			}
		}
		if(error)
		{
			return false;
		}
		routeCopy->removeFirstLink();
	}
	return true;
}

std::pair<std::vector<float>, std::vector<float>> City::routeToData(Route* route, float dist, SpeedPrediction* sp, Eigen::MatrixXd* spdIn) {

    std::vector<float> elevData;
    std::vector<float> spdData;
    
    Eigen::MatrixXd spdIn_i(*spdIn);

    bool isFirstLink = true;
	route->getLinks()->initializeCounter();
	GenericEntry<long int, Link*>* nextLink = route->getLinks()->nextEntry();
	while(nextLink != NULL && !nextLink->value->isFinalLink())
	{
        // get road from link
        assert(this->roads->hasEntry(nextLink->value->getNumber()));
        Road* roadFromLink = this->roads->getEntry(nextLink->value->getNumber());
    
        float distAlongLink = 0.0;
        if(isFirstLink)
        {
            distAlongLink = dist;
            isFirstLink = false;
        }
        
        // get spd and elevation data
        sp->setVals(nextLink->value->getWeights(nextLink->value->getDirection()));
        std::pair<std::vector<float>, std::vector<float>> linkData = this->getData(roadFromLink, nextLink->value->getDirection(), sp, &spdIn_i, distAlongLink);
        std::vector<float> spdOut_i = linkData.first;
        std::vector<float> elevData_i = linkData.second;
        
        // adjust speed input for next link speed prediction
        Eigen::MatrixXd spdOutMat_i(1, spdOut_i.size());
        for(int i = 0; i < spdOut_i.size(); i++) { spdOutMat_i.coeffRef(0, i)  = spdOut_i.at(i); }
        sp->output2Input(&spdIn_i, &spdOutMat_i);

        // concatenate data
        for(int i = 0; i < spdOut_i.size(); i++) { spdData.push_back(spdOut_i.at(i)); }
        for(int i = 0; i < elevData_i.size(); i++) { elevData.push_back(elevData_i.at(i)); }
        
		nextLink = route->getLinks()->nextEntry();
	}
    delete(nextLink);
    return std::pair<std::vector<float>, std::vector<float>>(spdData, elevData);
}

GenericMap<int, Bounds*>* City::getBoundsMap() {
	return this->boundsMap;
}

GenericMap<long int, Road*>* City::getRoads() {
	return this->roads;
}

int City::getBoundsMapSize() {
	return this->boundsMap->getSize();
}

GenericMap<long int, Intersection*>* City::getIntersections() {
	return this->intersections;
}
    
void City::addRoad(Road* road)
{
    this->roads->addEntry(road->getRoadID(), road);
}
    
void City::addBounds(Bounds* bounds)
{
    this->boundsMap->addEntry(bounds->getID(), bounds);
}
    
void City::addIntersection(Intersection* intersection)
{
    this->intersections->addEntry(intersection->getIntersectionID(), intersection);
}
    
Route* City::getRouteFromGPSTrace(GenericMap<long int, std::pair<double, double>*>* trace)
{
    GPS gps;
    Road* prevRoad = NULL;
    Road* currRoad = NULL;
    Intersection* nextIntersection = NULL;
    
    GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
    
    GenericMap<long int, std::pair<double, double>*>* traceCopy = trace->copy();
    
    long int linkCount = 0;
    bool isFirstRoad = true;
    
    // create new csv
    std::string csvName = "/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/TRACE.csv";
    FILE* csv = std::fopen(csvName.c_str(), "w");
    
    // add header to csv
    fprintf(csv, "name, description, color, latitude, longitude\n");

    trace->initializeCounter();
    GenericEntry<long int, std::pair<double, double>*>* nextMeas = trace->nextEntry();
    double lat = nextMeas->value->first;
    double lon = nextMeas->value->second;
    
    prevRoad = gps.getCurrentRoad2(this, lat, lon);
    
    nextMeas = trace->nextEntry();
    
    while(nextMeas != NULL)
    {
        // to csv
        fprintf(csv, "%ld,", nextMeas->key);
        fprintf(csv, "Lat & Lon: %.12f %.12f,", lat, lon);
        fprintf(csv, "red,");
        fprintf(csv, "%.12f,%.12f\n", lat, lon);
        
        lat = nextMeas->value->first;
        lon = nextMeas->value->second;
        
        currRoad = gps.getCurrentRoad2(this, lat, lon);
        
        if(currRoad == NULL)
        {
            nextMeas = trace->nextEntry();
            continue;
        }
        else if(prevRoad == NULL)
        {
            prevRoad = currRoad;
            nextMeas = trace->nextEntry();
            continue;
        }
        
        if(currRoad->getRoadID() != prevRoad->getRoadID())
        {
            // ensure previous road was on trace
            bool prevRoadIsOnTrace = this->roadIsOnTrace(prevRoad, traceCopy);
            bool currRoadIsOnTrace = this->roadIsOnTrace(currRoad, traceCopy);
            
            if((prevRoadIsOnTrace && currRoadIsOnTrace) || isFirstRoad)
            {
                isFirstRoad = false;
                Intersection* start = prevRoad->getStartIntersection();
                Intersection* end = prevRoad->getEndIntersection();
                
                float toStartIntDist = gps.deltaLatLonToXY(lat, lon, start->getLat(), start->getLon());
                float toEndIntDist = gps.deltaLatLonToXY(lat, lon, end->getLat(), end->getLon());
                
                Link* link;
                Intersection* currInt;
                
                if(toStartIntDist < toEndIntDist)
                {
                    link = Link().linkFromRoad(prevRoad, start);
                    currInt = start;
                }
                else
                {
                    link = Link().linkFromRoad(prevRoad, end);
                    currInt = end;
                }
                
                if(currInt->getIntersectionID() == currRoad->getStartIntersection()->getIntersectionID())
                {
                    nextIntersection = currRoad->getEndIntersection();
                }
                else
                {
                    nextIntersection = currRoad->getStartIntersection();
                }
                
                // ensure link does not already exist
                bool linkAlreadyExists = false;
                links->initializeCounter();
                GenericEntry<long int, Link*>* nextLink = links->nextEntry();
                while(nextLink !=  NULL)
                {
                    if(nextLink->value->isEqual(link))
                    {
                        linkAlreadyExists = true;
                        break;
                    }
                    nextLink = links->nextEntry();
                }
                delete(nextLink);
                
                if(!linkAlreadyExists)
                {
                    links->addEntry(linkCount, link);
                    linkCount++;
                }
                
                prevRoad = currRoad;
            }
        }
        
        nextMeas = trace->nextEntry();
    }
    delete(nextMeas);
    
    delete(traceCopy);
    
    fclose(csv);
    
    // add last road
    Link* link = Link().linkFromRoad(currRoad, nextIntersection);
    links->addEntry(linkCount++, link);
    
    // add final link
    links->addEntry(linkCount, Link().finalLink());
    
    Route* route = new Route(links, new Goal(nextIntersection->getIntersectionID()));
    return route;
}
    
bool City::roadIsOnTrace(Road* road, GenericMap<long int, std::pair<double, double>*>* trace)
{
    GPS gps;
    
    int closeNodeCount = 0;
    road->getNodes()->initializeCounter();
    GenericEntry<long int, Node*>* prevNode = road->getNodes()->nextEntry();
    GenericEntry<long int, Node*>* currNode = road->getNodes()->nextEntry();
    while(currNode != NULL)
    {
        trace->initializeCounter();
        GenericEntry<long int, std::pair<double, double>*>* prevMeasCopy = trace->nextEntry();
        GenericEntry<long int, std::pair<double, double>*>* currMeasCopy = trace->nextEntry();
        while(currMeasCopy != NULL)
        {
            double currRoadLat = currNode->value->getLat();
            double currRoadLon = currNode->value->getLon();
            
            double currTraceLat = currMeasCopy->value->first;
            double currTraceLon = currMeasCopy->value->second;
            
            float dist = gps.deltaLatLonToXY(currRoadLat, currRoadLon, currTraceLat, currTraceLon);
            
            // check proximity
            if(dist < gps.getDeltaXYTolerance())
            {
                // check heading
                double prevRoadLat = currNode->value->getLat();
                double prevRoadLon = currNode->value->getLon();
                
                double prevTraceLat = currMeasCopy->value->first;
                double prevTraceLon = currMeasCopy->value->second;
                
                double roadAngle = std::atan2(prevRoadLat - currRoadLat, prevRoadLon - currRoadLon);
                double traceAngle = std::atan2(prevTraceLat - currTraceLat, prevTraceLon - currTraceLon);
                
                if(roadAngle < 0)
                {
                    roadAngle += M_PI;
                }
                
                if(traceAngle < 0)
                {
                    traceAngle += M_PI;
                }
                
                if(std::abs(roadAngle - traceAngle) < M_PI / 4)
                {
                    closeNodeCount++;
                }
                break;
            }
            
            prevMeasCopy = currMeasCopy;
            currMeasCopy = trace->nextEntry();
        }
        delete(prevMeasCopy);
        delete(currMeasCopy);
        
        prevNode = currNode;
        currNode = road->getNodes()->nextEntry();
    }
    delete(prevNode);
    delete(currNode);
    
    return closeNodeCount > .50 * (float) road->getNodes()->getSize();
}
    
void City::printIntersectionsAndRoads()
{
    // csv name
    std::string csvName = "/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/CITY_INTERSECTIONS_AND_ROADS.csv";
    
    // delete existing csv if found
    std::string rm = "rm " + csvName;
    system(rm.c_str());
    
    // create new csv
    FILE* csv;
    csv = std::fopen(csvName.c_str(), "w");
    
    // add header to csv
    fprintf(csv, "name,icon,description,color,latitude,longitude\n");
    
    std::cout << "**** printing intersection lat/lon ****" << std::endl;
    this->intersections->initializeCounter();
    GenericEntry<long int, Intersection*>* nextInt = this->intersections->nextEntry();
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
        
        nextInt = this->intersections->nextEntry();
    }
    delete(nextInt);
    fclose(csv);
}

} /* namespace PredictivePowertrain */
