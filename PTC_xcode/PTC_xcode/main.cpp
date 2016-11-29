/*
 * main.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: vagrant
 */

#include <assert.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iostream>
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

#include "BuildCity.h"
#include "City.h"
#include "DataManagement.h"
#include "DriverPrediction.h"
#include "GPS.h"
#include "Kinematics.h"
#include "RoutePrediction.h"
#include "Route.h"
#include "UnitTests.h"
#include "VehicleDiagnostics.h" 


using namespace PredictivePowertrain;

// ************************************************************************************************************************************
// ************************************************************************************************************************************
//                                                   DP Diagnostics Reporting
// ************************************************************************************************************************************
// ************************************************************************************************************************************

// save actual route and speed
void saveActualData(Route* actualRoute,
                    std::vector<float>* actualSpeed,
                    std::vector<float>* fuelFlow,
                    std::vector<float>* energy,
                    std::vector<float>* time,
                    City* city)
{
    // ROUTE
    FILE* csvRoute = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_ACTUAL_ROUTE.csv", "w");
    actualRoute->saveRoute2CSV(csvRoute, city, true);
    
    // SPEED FUEL FLOW and CALCULATED ENERGY
    FILE* csvSpeedFuelFlowEnergyTime = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_ACTUAL_SPEED_FUEL_FLOW.csv", "w");

    // speed
    for(int i = 0; i < actualSpeed->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergyTime, "%f,", actualSpeed->at(i));
    }
    fprintf(csvSpeedFuelFlowEnergyTime, "\n");
    
    // fuel flow
    for(int i = 0; i < fuelFlow->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergyTime, "%f,", fuelFlow->at(i));
    }
    fprintf(csvSpeedFuelFlowEnergyTime, "\n");
    
    // calculated energy from prediction data
    for(int i = 0; i < energy->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergyTime, "%f,", energy->at(i));
    }
    fprintf(csvSpeedFuelFlowEnergyTime, "\n");
    
    // time
    for(int i = 0; i < time->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergyTime, "%f,", time->at(i));
    }
    
    fclose(csvSpeedFuelFlowEnergyTime);
}

// save pred data
void savePredData(DriverPrediction::PredData predData, Route* predRoute, City* city, FILE* predDataFile, FILE* predRouteFile)
{
    // route
    predRoute->saveRoute2CSV(predRouteFile, city, false);
    
    // other prediction data
    fprintf(predDataFile, "--- spd n ele ---\n");
    
    // pred speed
    for(int i = 0; i < predData.first.size(); i++)
    {
        fprintf(predDataFile, "%f,", predData.first.at(i));
    }
    fprintf(predDataFile, "\n");

    // pred elevation
    for(int i = 0; i < predData.second.size(); i++)
    {
        fprintf(predDataFile, "%f,", predData.second.at(i));
    }
    fprintf(predDataFile, "\n");
}

int main() {
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                          Unit Tests
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //    city_ut();
    //    buildCity_ut();
    //    dataCollection_ut();
    //    dataManagement_ut();
    //    driverPrediction_ut();
    //    GPS_ut();
    //    kinematics_ut();
    //    routePrediction_ut();
    //    speedPrediction_ut();
    //    vehicleDiagnostics_ut();
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                      Get Serialized Data
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
    // create data manager
    DataManagement dm;
    
    // get city data
    City* city = dm.getCityData();
    
    // get route prediction data
    RoutePrediction* rp = dm.getRoutePredictionData();
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                          Initiate
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
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
    Road* currRoad = NULL;
    Link* currLink = NULL;
    
    // kinematics
    Kinematics kin;
    
    // get distance along road
    float distAlongRoad = 0.0;
    
    // start driver prediction
    std::vector<float> currConditions(2);
    currConditions.at(0) = 1;
    currConditions.at(1) = 2;
    
    // vehicle speed;
    float vehSpd;
    float totalDist = 0;
    
    // vehicle speed prediction distance between predictions
    float ds = SpeedPrediction().getDS();
    
    // ensure heading is along road spline trajectory
    bool headingIsStart2End = true;
    
    // containers for actual fuel and speed
    std::vector<float> actualSpeed;
    std::vector<float> fuelFlow;
    std::vector<float> energy;
    std::vector<float> time;
    
    FILE* predDataFile = fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_PRED_DATA.csv", "w");
    FILE* predRouteFile = fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_PRED_ROUTE.csv", "w");
    fprintf(predRouteFile, "name, description, color, latitude, longitude\n");
    
    // ************************************************************************************************************************************
    // ******************************************************************v*****************************************************************
    //                                                      Run Program
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
    // record trip data to build map
    if(city == NULL)
    {
        std::string line;
        while(vd.getEngineLoad() > 1.0)
        {
            auto start = std::chrono::system_clock::now();
            while(1)
            {
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> diff = end - start;
                
                if(diff.count() > 1.0) { break; }
            }
            
            gps.updateTripLog();
        }
    }
    else
    {
        // to start driver prediction
        bool isFirstPrediction = true;
        
        auto start = std::chrono::system_clock::now();
        std::pair<double, double> prevLatLon = gps.readGPS();
        
        // while car is running
        while(vd.getEngineLoad() > 1.0)
        {
            // get vehicle speed
            vehSpd = vd.getSpeed();
            std::cout << "veh speed: " << vehSpd << std::endl;
            actualSpeed.push_back(vehSpd);
            
            // get fuel flow
            fuelFlow.push_back(vd.getFuelFlow());
            
            // update current road if intersection happen
            if(!gps.isOnRoad(currRoad))
            {
                currRoad = gps.getCurrentRoad1(city);
                headingIsStart2End = gps.isHeadingStart2EndOfCurrentRoad(currRoad);
                
                std::cout << "on new road: " << currRoad->getRoadID() << std::endl;
                
                if(headingIsStart2End)
                {
                    currLink = Link().linkFromRoad(currRoad, currRoad->getStartIntersection());
                }
                else
                {
                    currLink = Link().linkFromRoad(currRoad, currRoad->getEndIntersection());
                }
                
                // start prediction
                if(isFirstPrediction)
                {
                    // get distance along road
                    distAlongRoad = gps.getDistAlongRoad(currRoad, true, headingIsStart2End);
                    predData = dp.startPrediction(currLink, vehSpd, &currConditions, distAlongRoad);
                    isFirstPrediction = false;
                }
                
                if(predData.first.at(0) != -1 && predData.second.at(0) != -1)
                {
                    savePredData(predData, dp.getRP()->getPredictedRoute(), city, predDataFile, predRouteFile);
                }
            }
            
            // get distance along road
            distAlongRoad = gps.getDistAlongRoad(currRoad, true, headingIsStart2End);

            // predict iteratively along vehicle route
            predData = dp.nextPrediction(currLink, vehSpd, distAlongRoad);
            
            // approximate energy usage from predicted speed and elevation change
            if(predData.first.at(0) != -1 && predData.second.at(0) != -1)
            {
                energy.push_back(kin.runKinematics(predData.first, ds, predData.second, false));
            }
            
            // ensure vehicle has traveled prediction interval distance before next prediction
            while(vd.getEngineLoad() > 1.0)
            {
                std::pair<double, double> currLatLon = gps.readGPS();
                float travelDist_i = gps.deltaLatLonToXY(prevLatLon.first, prevLatLon.second, currLatLon.first, currLatLon.second);
                
                if(travelDist_i > ds)
                {
                    // get left over dist from previous dist measurement and add to current dist measurement
                    float distRatio = (std::fmodf(totalDist, ds) + travelDist_i) / ds;
                    
                    totalDist += travelDist_i;
                    std::cout << "total dist traveled: " << totalDist << std::endl;
                    
                    // if dist ratio is more than 2x prediction interval distance, buffer speed
                    for(int i = 1; i <= distRatio; i++)
                    {
                        actualSpeed.push_back(vehSpd);
                        dp.updateSpeedsbyVal(vehSpd);
                    }
                    
                    std::cout << "prediction distance: " << travelDist_i << std::endl;
                    std::cout << "**************************************************************************************" << std::endl;
                    prevLatLon = currLatLon;
                    break;
                }
            }
            
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end - start;
            time.push_back(diff.count());
            start = end;
        }
    }
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                  Store Data & Shutdown
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
    // add -1 to end of speed trace for later parsing
    actualSpeed.push_back(-1);
    
    // create build city module to add new data if needed
    BuildCity bc;
    
    // get and store trip data
    dm.addTripData(gps.getTripLog(true));
    
    // update city data
    city = bc.getUpdatedCity();
    
    // store city data
    dm.addCityData(city);

    // get route from city using gps trace
    Route* actualRoute = city->getRouteFromGPSTrace(gps.getTripLog(true));
    
    // save actual route
    saveActualData(actualRoute, &actualSpeed, &fuelFlow, &energy, &time, city);
    
    // parse route
    dp.parseRoute(actualRoute, &actualSpeed, gps.getTripLog(true));
    
    // store route prediction data
    dm.addRoutePredictionData(dp.getRP());
    
    fclose(predRouteFile);
    fclose(predDataFile);

	std::cout << "finished driver prediction" << std::endl;

	return 0;
}
