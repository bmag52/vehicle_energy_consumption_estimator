/*
 * DataManagement_ut.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: vagrant
 */


#include "UnitTests.h"
#include "../data_management/DataManagement.h"
#include "../driver_prediction/DriverPrediction.h"
#include "../route_prediction/RoutePrediction.h"

using namespace PredictivePowertrain;

void dataManagement_ut()
{
    float version = 2.0;
    
    DataManagement testDM;
    
    if(version == 2.0)
    {
        // print city
        City* cityStore = testDM.getCityData();
        cityStore->printIntersectionsAndRoads();
        testDM.addCityData(cityStore);
        
        // get route prediction
        RoutePrediction* rpStore = testDM.getRoutePredictionData();
        rpStore->addCity(cityStore);
        
        GenericMap<long int, std::pair<double, double>*>* trace = testDM.getMostRecentTripData();
        Route* route = cityStore->getRouteFromGPSTrace(trace);
        Route* route2 = route->copy();
        
        std::ifstream input("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_ACTUAL_SPEED_FUEL_FLOW.csv");
        
        std::string num;
        std::vector<float> actualSpds;
        
        // read in speed from csv
        while(1)
        {
            std::getline(input, num, ',');
            std::stringstream fs(num);
            float f = 0.0;
            fs >> f;
            
            if(f == -1)
            {
                break;
            }
            
            actualSpds.push_back(f);
        }
        
        DriverPrediction dp(rpStore);
        dp.parseRoute(route, &actualSpds, trace);
        
        testDM.addRoutePredictionData(dp.getRP());
    }
        
    if(version == 1.0)
    {
        // test trip log add and get
        GenericMap<long int, std::pair<double, double>*>* latLon = new GenericMap<long int, std::pair<double, double>*>();
        latLon->addEntry(1, new std::pair<double, double>(47.654, -122.345));
        latLon->addEntry(2, new std::pair<double, double>(47.654, -122.345));
        latLon->addEntry(3, new std::pair<double, double>(47.654, -122.345));
        testDM.addTripData(latLon);
        testDM.addTripData(latLon);
        GenericMap<long int, std::pair<double, double>*>* storedLatLon = testDM.getMostRecentTripData();
        assert(storedLatLon->getSize() == latLon->getSize());

        storedLatLon->initializeCounter();
        bool latLonsMatch = true;
        GenericEntry<long int, std::pair<double, double>*>* nextStoredLatLon = storedLatLon->nextEntry();
        while(nextStoredLatLon != NULL)
        {
            if(!latLon->hasEntry(nextStoredLatLon->key) || nextStoredLatLon->value->first != latLon->getEntry(nextStoredLatLon->key)->first
                    || nextStoredLatLon->value->second != latLon->getEntry(nextStoredLatLon->key)->second)
            {
                latLonsMatch = false;
            }
            nextStoredLatLon = storedLatLon->nextEntry();
        }
        assert(latLonsMatch);

        // test city logging add old test
        GenericMap<long int, Node*>* nodes = new GenericMap<long int, Node*>();
        nodes->addEntry(1, new Node(42.3, -122.4, 44, 1234566));
        nodes->addEntry(2, new Node(42.3, -122.4, 44, 1234566));

        GenericMap<long int, Road*>* blankRoads;
        GenericMap<int, Bounds*>* boundsMap = new GenericMap<int, Bounds*>();
        boundsMap->addEntry(0, new Bounds(2, 3, 4, 5));
        boundsMap->addEntry(1, new Bounds(2, 3, 4, 5));
        boundsMap->addEntry(2, new Bounds(2, 3, 4, 5));

        Intersection* startInt = new Intersection(blankRoads, 42.3, -122.4, 44, 345);
        Intersection* endInt = new Intersection(blankRoads, 42.3, -122.4, 44, 456);

        Road* road1 = new Road("hilly", 987654, nodes);
        road1->setStartIntersection(startInt);
        road1->setEndIntersection(endInt);
        road1->setBoundsID(0);

        Road* road2 = new Road("flat", 8765, nodes);
        road2->setStartIntersection(startInt);
        road2->setEndIntersection(endInt);
        road2->setBoundsID(1);

        Road* road3 = new Road("medium", 9854, nodes);
        road3->setStartIntersection(startInt);
        road3->setEndIntersection(endInt);
        road3->setBoundsID(2);

        GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
        roads->addEntry(road1->getRoadID(), road1);
        roads->addEntry(road2->getRoadID(), road2);
        roads->addEntry(road3->getRoadID(), road3);

        Intersection* intersection1 = new Intersection(roads, 42.3, -122.4, 44, 123);
        intersection1->setBoundsID(0);
        Intersection* intersection2 = new Intersection(roads, 42.3, -122.4, 44, 234);
        intersection2->setBoundsID(0);
        Intersection* intersection3 = new Intersection(roads, 42.3, -122.4, 44, 345);
        intersection3->setBoundsID(1);
        Intersection* intersection4 = new Intersection(roads, 42.3, -122.4, 44, 456);
        intersection4->setBoundsID(2);

        GenericMap<long int, Intersection*>* intersections = new GenericMap<long int, Intersection*>();
        intersections->addEntry(intersection1->getIntersectionID(), intersection1);
        intersections->addEntry(intersection2->getIntersectionID(), intersection2);
        intersections->addEntry(intersection3->getIntersectionID(), intersection3);
        intersections->addEntry(intersection4->getIntersectionID(), intersection4);

        // store city
        City* city = new City(intersections, roads, boundsMap);
        testDM.addCityData(city);
        
        // add prediction data
        SpeedPrediction* sp = speedPrediction_ut();
        RoutePrediction* rp = routePrediction_ut();
        
        rp->getLinks()->initializeCounter();
        GenericEntry<long int, Link*>* nextLink = rp->getLinks()->nextEntry();
        while(nextLink != NULL)
        {
            std::vector<std::vector<Eigen::MatrixXd*>*>* nnData = sp->getVals();
            std::vector<Eigen::MatrixXd*>* wts = nnData->at(0); 
            std::vector<Eigen::MatrixXd*>* yHid = nnData->at(1);
            std::vector<Eigen::MatrixXd*>* yInHid = nnData->at(2);
            
            nextLink->value->setWeights(wts, yHid, yInHid, 1);
            nextLink->value->setWeights(wts, yHid, yInHid, 0);
            nextLink->value->setNumNNLayers(sp->getNumLayers());
            
            nextLink = rp->getLinks()->nextEntry();
        }
        
        testDM.addRoutePredictionData(rp);

        // retrieve stored city data
        City* storedCity = testDM.getCityData();
        
        // check sizes
        assert(city->getInstersectionMapSize() == storedCity->getInstersectionMapSize());
        assert(city->getRoadMapSize() == storedCity->getRoadMapSize());
        assert(city->getBoundsMapSize() == storedCity->getBoundsMapSize());

        // check roads
        GenericMap<long int, Road*>* storedRoads = storedCity->getRoads();
        bool hasAllRoads = true;
        storedRoads->initializeCounter();
        GenericEntry<long int, Road*>* nextStoredRoad = storedRoads->nextEntry();
        while(nextStoredRoad != NULL)
        {
            if(!roads->hasEntry(nextStoredRoad->key))
            {
                hasAllRoads = false;
            }
            assert(intersections->hasEntry(nextStoredRoad->value->getStartIntersection()->getIntersectionID()));
            assert(intersections->hasEntry(nextStoredRoad->value->getEndIntersection()->getIntersectionID()));
            nextStoredRoad = storedRoads->nextEntry();
        }
        assert(hasAllRoads);

        // check ints
        GenericMap<long int, Intersection*>* storedInts = storedCity->getIntersections();
        bool hasAllInts = true;
        storedInts->initializeCounter();
        GenericEntry<long int, Intersection*>* nextStoredInt = storedInts->nextEntry();
        while(nextStoredInt != NULL)
        {
            if(!intersections->hasEntry(nextStoredInt->key))
            {
                hasAllInts = false;
            }
            GenericMap<long int, Road*>* connectingRoads = nextStoredInt->value->getRoads();
            bool hasAllConnectingRoads = true;
            connectingRoads->initializeCounter();
            GenericEntry<long int, Road*>* nextStoredConnectingRoad = connectingRoads->nextEntry();
            while(nextStoredConnectingRoad != NULL)
            {
                if(!roads->hasEntry(nextStoredConnectingRoad->key))
                {
                    hasAllConnectingRoads = false;
                }
                nextStoredConnectingRoad = connectingRoads->nextEntry();
            }
            nextStoredInt = storedInts->nextEntry();
            assert(hasAllConnectingRoads);
        }
        assert(hasAllInts);

        // check bounds
        GenericMap<int, Bounds*>* storedBounds = storedCity->getBoundsMap();
        bool hasAllBounds = true;
        storedBounds->initializeCounter();
        GenericEntry<int, Bounds*>* nextStoredBounds = storedBounds->nextEntry();
        while(nextStoredBounds != NULL)
        {
            if(!boundsMap->hasEntry(nextStoredBounds->key))
            {
                hasAllBounds = false;
            }
            nextStoredBounds = storedBounds->nextEntry();
        }
        assert(hasAllBounds);
        
        // retrieve stored route prediction data
        RoutePrediction* storedrp = testDM.getRoutePredictionData();
    }
    
}

