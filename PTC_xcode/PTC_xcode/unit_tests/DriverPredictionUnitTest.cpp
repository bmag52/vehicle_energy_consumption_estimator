/*
 * DriverPrediction.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: vagrant
 */
#include "../route_prediction/RoutePrediction.h"
#include "../speed_prediction/SpeedPrediction.h"
#include "../city/City.h"
#include "UnitTests.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>


using namespace PredictivePowertrain;

void driverPrediction_ut()
{

    // seed random gen
    std::srand(std::time(0));
    
    // prediction distance interval
    float predDistInt = 5.0;
    
    RoutePrediction* rp = routePrediction_ut();
    SpeedPrediction* sp = speedPrediction_ut();
    
    // assign bounded random distances to roads in city
    City* city = rp->getCity();
    city->getRoads()->initializeCounter();
    GenericEntry<long int, Road*>* nextRoad = city->getRoads()->nextEntry();
    while(nextRoad != NULL)
    {
        // create random road distance between 1000 and 200 meters
        float roadDist = 1000 * (float) std::rand() / RAND_MAX + 200;
        
        // assign road dist
        nextRoad->value->assignSplineLength(roadDist);
        
        nextRoad = city->getRoads()->nextEntry();
    }
    delete(nextRoad);
    
    // input speed trace
    std::ifstream input("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/spd.csv");
    std::string num;
    
    std::vector<float> routeSpeed;
    // read in speed from csv
    for (int i = 0; i<10000; i++){
        std::getline(input, num, ',');
        std::stringstream fs(num);
        float f = 0.0;
        fs >> f;
        routeSpeed.push_back(f);
    }
    
    // get distance of speed measurements
    
}
