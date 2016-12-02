/*
 * City.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../city/City.h"
#include "../data_management/DataManagement.h"
#include "../route_prediction/Route.h"
#include "../route_prediction/RoutePrediction.h"
#include "../driver_prediction/DriverPrediction.h"

#include <assert.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <utility>
#include <cmath>
#include <math.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <vector>

using namespace PredictivePowertrain;

void city_ut()
{
    DataManagement dm;
    
    // print city
    City* city = dm.getCityData();
    city->printIntersectionsAndRoads();
    
    // get route prediction
    RoutePrediction* rp = dm.getRoutePredictionData();
    rp->addCity(city);
    
    GenericMap<long int, std::pair<double, double>*>* trace = dm.getMostRecentTripData();
    Route* route = city->getRouteFromGPSTrace(trace);
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
    
    DriverPrediction dp(rp);
    dp.parseRoute(route, &actualSpds, trace);
    
    dm.addRoutePredictionData(dp.getRP());
    
    FILE* csvRoute = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/UNIT_TEST_CITY_TRACE_2_ROUTE.csv", "w");
    route->saveRoute2CSV(csvRoute, city, true);
    
    // test rp across generated route
    std::cout << " --------------------- original rp --------------------- " << std::endl;
    
    std::vector<float>* conditions = new std::vector<float>(2);
    conditions->at(0) = 1;
    conditions->at(1) = 2;
    
    Intersection* startIntersection = city->getIntersectionFromLink(route->getLinks()->getEntry(0), true);
    
    int predIter = 1;
    Route* predRoute = rp->startPrediction(route2->getLinks()->getEntry(0), startIntersection, conditions);
    while(route->getLinkSize() > 2)
    {
        std::cout << "--- route prediction iteration " << predIter << " ---" << std::endl;
        
        // update actual route as it's 'driven' over
        route->removeFirstLink();
        
        // print actual route
        route->printLinks();
        
        // print predicted route
        predRoute->printLinks();
        
        // predict route
        predRoute = rp->predict(route->getLinks()->getEntry(0));
        
        // check if predicted route is actual route
//        if(route->isEqual(predRoute))
//        {
//            std::cout << "predicted routed before reaching end destination!" << std::endl;
//            std::cout << "Links traversed: " << predIter << std::endl;
//            std::cout << "Links in route: " << route->getLinkSize() << std::endl;
//            break;
//        }
        
        predIter++;
        
    }
    
    // test rp across stored route
    std::cout << " --------------------- stored rp --------------------- " << std::endl;
    RoutePrediction* rp2 = dm.getRoutePredictionData();
    rp2->addCity(city);
    
    predIter = 1;
    predRoute = rp2->startPrediction(route2->getLinks()->getEntry(0), startIntersection, conditions);
    while(route2->getLinkSize() > 2)
    {
        std::cout << "--- route prediction iteration " << predIter << " ---" << std::endl;
        
        // update actual route as it's 'driven' over
        route2->removeFirstLink();
        
        // print actual route
        route2->printLinks();
        
        // print predicted route
        predRoute->printLinks();
        
        // predict route
        predRoute = rp2->predict(route2->getLinks()->getEntry(0));
        
        // check if predicted route is actual route
//        if(route->isEqual(predRoute))
//        {
//            std::cout << "predicted routed before reaching end destination!" << std::endl;
//            std::cout << "Links traversed: " << predIter << std::endl;
//            std::cout << "Links in route: " << route->getLinkSize() << std::endl;
//            break;
//        }
        
        predIter++;
    }
    
    int test = 2;
    

}
