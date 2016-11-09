/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <assert.h>
#include <chrono>
#include <iostream>

#include "BuildCity.h"
#include "City.h"
#include "DataManagement.h"
#include "DriverPrediction.h"
#include "GPS.h"
#include "RoutePrediction.h"
#include "UnitTests.h"
#include "VehicleDiagnostics.h" 


using namespace PredictivePowertrain;

int main() {
    
    // *********************************
    // *********************************
    //           Unit Tests
    // *********************************
    // *********************************
    //    dataManagement_ut();
    //    dataCollection_ut();
    //    buildCity_ut();
    //    speedPrediction_ut();
    //    routePrediction_ut();
    //    kinematics_ut();
    //    GPS_ut();
    //    driverPrediction_ut();
    
    // *********************************
    // *********************************
    //        Get Serialized Data
    // *********************************
    // *********************************
    
    // create data manager
    DataManagement dm;
    
    // get city data
    City* city = dm.getCityData();
    
    // get route prediction data
    RoutePrediction* rp = dm.getRoutePredictionData();
    
    // *********************************
    // *********************************
    //            Initiate
    // *********************************
    // *********************************
    
    // add city to route prediction
    rp->addCity(city);
    
    // add route prediction to driver prediction
    DriverPrediction::PredData predData;
    DriverPrediction dp(rp);
    
    // create diagnostics model
    VehicleDiagnostics vd;
    
    // create gps model
    GPS gps;
    
    // current road vehicle is on
    Road* currRoad;
    Link* currLink;
    
    // get distance along road
    float distAlongRoad = 0.0;
    
    // start driver prediction
    std::vector<float> currConditions(2);
    currConditions.at(0) = 1;
    currConditions.at(1) = 2;
    
    // vehicle speed;
    float vehSpd;
    
    // vehicle speed prediction distance between predictions
    float ds = SpeedPrediction().getDS();
    
    // *********************************
    // *********************************
    //          Run Program
    // *********************************
    // *********************************
    
    bool isFirstPrediction = true;
    
    while(1)
    {
        // set timer between time measurements to ensure distance covered is correct
        auto t1 = std::chrono::system_clock::now();
        
        // get vehicle speed
        vehSpd = vd.getSpeed();
        
        // get current road / link and start driver prediction
        if(isFirstPrediction)
        {
            currRoad = gps.getCurrentRoad(city);
            currLink = Link().linkFromRoad(currRoad, currRoad->getStartIntersection());
            distAlongRoad = gps.getDistAlongRoad(currRoad);
            
            predData = dp.startPrediction(currLink, vehSpd, &currConditions, distAlongRoad);
            
            isFirstPrediction = false;
        }
        
        // routinely update road vehicle is on
        else
        {
            if(!gps.isOnRoad(currRoad))
            {
                currRoad = gps.getCurrentRoad(city);
                currLink = Link().linkFromRoad(currRoad, currRoad->getStartIntersection());
            }
            
            distAlongRoad = distAlongRoad = gps.getDistAlongRoad(currRoad);
            predData = dp.nextPrediction(currLink, vehSpd, distAlongRoad);
        }
        
        bool spdPredDistCovered = false;
        while(!spdPredDistCovered)
        {
            // get new speed prediction
            float vehSpdNew = vd.getSpeed();
            
            // get time update
            auto t2 = std::chrono::system_clock::now();
            std::chrono::duration<double> timeDur = t2 - t1;
            float dt = timeDur.count();
            
            float accel = (vehSpd - vehSpdNew) / dt;
            float dist = vehSpd * dt - 0.5 * accel * std::pow(dt, 2);
            
            if(dist > ds)
            {
            
                spdPredDistCovered = true;
            }
        }
    }
    
    // *********************************
    // *********************************
    //       Store Data & Shutdown
    // *********************************
    // *********************************
    
    // create build city module to add new data if needed
    BuildCity bc;
    
    // get and store trip data
    dm.addTripData(gps.getTripLog());
    
    // update city data
    bc.updateGridDataXMLSpline();
    bc.printNewIntersectionsAndRoads();
    
    // store city data
    dm.addCityData(bc.getUpdatedCity());
    
    // store route prediction data
    dm.addRoutePredictionData(rp);

	std::cout << "finished" << std::endl;

	return 0;
}
