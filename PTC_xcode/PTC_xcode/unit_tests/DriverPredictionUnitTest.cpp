/*
 * DriverPrediction.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: vagrant
 */

#include "../route_prediction/Goal.h"
#include "../route_prediction/Route.h"
#include "../driver_prediction/DriverPrediction.h"
#include "../route_prediction/RoutePrediction.h"
#include "../speed_prediction/SpeedPrediction.h"
#include "../data_management/DataManagement.h"
#include "../city/BuildCity.h"
#include "../city/City.h"
#include "UnitTests.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

using namespace PredictivePowertrain;

void setRouteNeuralNetworkVals(Route* route, std::vector<float> spds, float spdDist, City* city)
{
    std::cout << "Driver Prediction Speed Training" << std::endl;
    DriverPrediction dp;
    
    std::vector<float> spdsTemp(spds);
    
    // add neural net values to route prediction links
    GenericMap<long int, Link*>* links = route->getLinks();
    
    links->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = links->nextEntry();
    while(nextLink != NULL && !nextLink->value->isFinalLink())
    {
        // get necessary road data
        Road* road_i = city->getRoads()->getEntry(nextLink->value->getNumber());
        float roadDist_i = road_i->getSplineLength();
        float roadSpdIndices_i = (float) roadDist_i / spdDist * spds.size();
        
        std::vector<float> roadSpds_i;
        
        // get road speeds from arbitrary trace of continuous speed values.
        for(int i = 0; i < roadSpdIndices_i; i++)
        {
            if(spdsTemp.size() == 0)
            {
                for(int j = 0; j < spds.size(); j++)
                {
                    spdsTemp.push_back(spds.at(j));
                }
            }
            
            roadSpds_i.push_back(spdsTemp.at(0));
            spdsTemp.erase(spdsTemp.begin());
        }
        
        // train for speed at beginnging of next link
        roadSpds_i.push_back(spdsTemp.at(0));
        
        std::cout << roadSpds_i.size() << std::endl;
        
        dp.setCurrentLink(nextLink->value);
        
        while(roadSpds_i.size() > 0)
        {
            dp.getSpeedPredInpunt(roadSpds_i.front());
            roadSpds_i.erase(roadSpds_i.begin());
        }
        
        dp.trainSpeedPredictionOverLastLink();
        
        nextLink = links->nextEntry();
    }
    delete(nextLink);
}

void driverPrediction_ut()
{
    // -----------------------------------
    // BUILD SMALL CITY SECTION IN SPOKANE
    // -----------------------------------
    
    DataManagement dm;
    bool getCityFromDM = true;
    
    City* rpCity;
    
    if(!getCityFromDM)
    {
        // make trip log
        GenericMap<long int, std::pair<double, double>*>* latLon = new GenericMap<long int, std::pair<double, double>*>();
        
        // spokane trip log small
        latLon->addEntry(1, new std::pair<double, double>(47.634, -117.396));
        latLon->addEntry(2, new std::pair<double, double>(47.635, -117.397));
        latLon->addEntry(3, new std::pair<double, double>(47.636, -117.398));
        latLon->addEntry(4, new std::pair<double, double>(47.637, -117.399));
        latLon->addEntry(5, new std::pair<double, double>(47.638, -117.400));
        latLon->addEntry(6, new std::pair<double, double>(47.639, -117.401));
        latLon->addEntry(7, new std::pair<double, double>(47.640, -117.402));
        latLon->addEntry(8, new std::pair<double, double>(47.645, -117.406));
        
        // jsonify trip log -> delete existing jsons
        dm.addTripData(latLon);
        
        BuildCity bc;
        bc.updateGridDataXMLSpline();
        bc.printNewIntersectionsAndRoads();

        GenericMap<long int, Intersection*>* intersections = bc.getNewIntersections();
        GenericMap<long int, Road*>* roads = bc.getNewRoads();
        GenericMap<int, Bounds*> bounds;
        bounds.addEntry(0, bc.getNewBounds());

        rpCity = new City(intersections, roads, &bounds);
        dm.addCityData(rpCity);
    }
    else
    {
        rpCity = dm.getCityData();
    }
    
    // ----------------------
    // SETUP SPEED PREDICTION
    // ----------------------
    
    SpeedPrediction sp;
    
    std::ifstream input("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/spd.csv");
    std::string num;
    
    // speeds for routes, both actual and random
    std::vector<float> routeSpds;
    
    // distance covered by speed predictions
    float routeSpdsDist = 0.0;
    
    // fill actual speed with EPA drive cycle speed traces
    for (int i = 0; i<17000; i++)
    {
        std::getline(input, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        routeSpds.push_back(f);
        routeSpdsDist += sp.getDS();
    }
    
    // ----------------------
    // SETUP ROUTE PREDICTION
    // ----------------------
    
    // set route length
    int routeLength = 10;
    
    // set conditions
    std::vector<float>* conditions = new std::vector<float>(2);
    conditions->at(0) = 1;
    conditions->at(1) = 2;
    
    Intersection* startIntersection = rpCity->getIntersections()->getFirstEntry();
    Intersection* endIntersection = rpCity->getIntersections()->getLastEntry();
    Link* firstLink = new Link(1, startIntersection->getRoads()->getFirstEntry()->getRoadID());

    RoutePrediction rp(rpCity);
    
    // generate random actual route
    Route startRoute;
    startRoute.addLink(firstLink);
    startRoute.assignGoal(new Goal(endIntersection->getIntersectionID()));
    Route* actualRoute = rpCity->randomPath(startIntersection, &startRoute, routeLength, conditions);
    
    // add NN vals
    setRouteNeuralNetworkVals(actualRoute, routeSpds, routeSpdsDist, rpCity);
    
    // add trainging iterations here (simulates driving over the route multiple times)
    rp.parseRoute(actualRoute);
    rp.parseRoute(actualRoute);
    rp.parseRoute(actualRoute);
    rp.parseRoute(actualRoute);
    rp.parseRoute(actualRoute);
    
    // create number of random routes to include in test set
    int num_rand_routes = 4;
    
    for(int i = 1; i <= num_rand_routes; i++)
    {
        // create random route
        Route* randomRoute = rpCity->randomPath(startIntersection, &startRoute, std::ceil((float)std::rand() / RAND_MAX * routeLength), conditions);
        while(randomRoute->isEqual(actualRoute))
        {
            randomRoute = rpCity->randomPath(startIntersection, &startRoute, std::ceil((float)std::rand() / RAND_MAX * routeLength), conditions);
        }
        
        // add NN vals
        setRouteNeuralNetworkVals(randomRoute, routeSpds, routeSpdsDist, rpCity);
        
        // add random route to test set
        rp.parseRoute(randomRoute);
    }
    
    // -----------------------
    // BEGIN DRIVER PREDICTION
    // -----------------------
    
    DriverPrediction dp(&rp);
    DriverPrediction::PredData predData;
    
    std::vector<float> routeSpdsTmp(routeSpds);
    std::ofstream predFile("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/driverPredictionResults.csv");
    
    bool isFirstLink = true;
    int predictionCount = 0;
    float distanceAlongRoute = 0.0;
    
    // begin driving actual route
    while(actualRoute->getLinks()->getSize() > 0)
    {
        Link* nextLink = actualRoute->getLinks()->getEntry(0);
        actualRoute->removeFirstLink();
        
        // break if final link is found
        if(nextLink->isFinalLink())
        {
            std::cout << "Driver Prediction Unit Test Complete" << std::endl;
            break;
        }
        
        // get road data from actual route
        Road* road_i = rpCity->getRoads()->getEntry(nextLink->getNumber());
        float roadDist_i = road_i->getSplineLength();
        float distAlongLink = 0.0;
        
        // test handling of starting route partially up first link
        if(isFirstLink)
        {
            distAlongLink = roadDist_i / 2;
            distanceAlongRoute += distAlongLink;
            
            float removeBeforeIndex = (float) distAlongLink / routeSpdsDist * routeSpds.size();
            for(int i = 0; i < removeBeforeIndex; i++)
            {
                routeSpdsTmp.erase(routeSpdsTmp.begin());
            }
            std::cout << "Starting Driver Prediciton" << std::endl;
            predData = dp.startPrediction(nextLink, routeSpdsTmp.front(), conditions, distAlongLink);
            
            // udate distance along link
            distAlongLink += sp.getDS();
            distanceAlongRoute += sp.getDS();
            
            // actual route speeds
            predFile << "Actual Route Speeds\n";
            for(int i = 0; i < routeSpdsTmp.size(); i++)
            {
                predFile << routeSpdsTmp.at(i);
                predFile << ",";
            }
            predFile << "\n";
            
            routeSpdsTmp.erase(routeSpdsTmp.begin());
            isFirstLink = false;
        }
        
        while(distAlongLink < roadDist_i)
        {
            predFile << "Prediction: " << predictionCount << "\n";
            predictionCount++;
            
            // predicted speed
            for(int i = 0; i < predData.first.size(); i++)
            {
                predFile << predData.first.at(i);
                predFile << ",";
            }
            predFile << "\n";
            
            // predicted elevation
            for(int i = 0; i < predData.second.size(); i++)
            {
                predFile << predData.second.at(i);
                predFile << ",";
            }
            predFile << "\n";
            
            if(routeSpdsTmp.size() == 0)
            {
                for(int j = 0; j < routeSpds.size(); j++)
                {
                    routeSpdsTmp.push_back(routeSpds.at(j));
                }
            }
            
            // make next prediction
            predData = dp.nextPrediction(nextLink, routeSpdsTmp.front(), distAlongLink);
            
            // udate distance along link
            distAlongLink += sp.getDS();
            distanceAlongRoute += sp.getDS();
            
            std::cout << distanceAlongRoute << std::endl;
            
            // update actual speed values along actual route
            routeSpdsTmp.erase(routeSpdsTmp.begin());
        }
        
        // print routes
        std::cout << "---- route prediciton ----" << std::endl;
        actualRoute->printLinks();
        dp.getRP()->getPredictedRoute()->printLinks();
        

    }
}

