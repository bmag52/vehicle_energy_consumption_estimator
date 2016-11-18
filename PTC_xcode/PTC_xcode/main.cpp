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
// save route
void saveRoute(Route* route, FILE* file, City* city)
{
    route->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = route->getLinks()->nextEntry();
    while(nextLink != NULL)
    {
        
        Road* currRoad = city->getRoads()->getEntry(nextLink->value->getNumber());
        
        // intersections
        for(int i = 0; i < 2; i++)
        {
            Intersection* intersection = currRoad->getStartIntersection();
            if(i == 1)
            {
                intersection = currRoad->getEndIntersection();
            }
            
            // intersections
            fprintf(file, "%ld,", intersection->getIntersectionID());
            fprintf(file, "Lat & Lon: %.12f %.12f,", intersection->getLat(), intersection->getLon());
            fprintf(file, "blue,");
            fprintf(file, "%.12f,%.12f\n", intersection->getLat(), intersection->getLon());
        }
        
        currRoad->getNodes()->initializeCounter();
        GenericEntry<long int, Node*>* nextNode = currRoad->getNodes()->nextEntry();
        while(nextNode != NULL)
        {
            // road nodes
            fprintf(file, "%ld,", currRoad->getRoadID());
            fprintf(file, "Lat & Lon: %.12f %.12f,", nextNode->value->getLat(), nextNode->value->getLon());
            fprintf(file, "red,");
            fprintf(file, "%.12f,%.12f\n", nextNode->value->getLat(), nextNode->value->getLon());
            nextNode = currRoad->getNodes()->nextEntry();
        }
        delete(nextNode);
        
        nextLink = route->getLinks()->nextEntry();
    }
    delete(nextLink);
}

// save route and speed
void saveActualData(Route* actualRoute, std::vector<float>* actualSpeed, std::vector<float>* fuelFlow, std::vector<float>* energy, City* city)
{
    // ROUTE
    FILE* csvRoute = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_ACTUAL_ROUTE.csv", "w");
    
    fprintf(csvRoute, "name, description, color, latitude, longitude\n");
    saveRoute(actualRoute, csvRoute, city);
    fclose(csvRoute);
    
    // SPEED
    FILE* csvSpeedFuelFlowEnergy = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_ACTUAL_SPEED_FUEL_FLOW.csv", "w");
    
    for(int i = 0; i < actualSpeed->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergy, "%f,", actualSpeed->at(i));
    }
    
    fprintf(csvSpeedFuelFlowEnergy, "\n");
    
    for(int i = 0; i < fuelFlow->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergy, "%f,", fuelFlow->at(i));
    }
    
    fprintf(csvSpeedFuelFlowEnergy, "\n");
    
    for(int i = 0; i < energy->size(); i++)
    {
        fprintf(csvSpeedFuelFlowEnergy, "%f,", energy->at(i));
    }
    
    fclose(csvSpeedFuelFlowEnergy);
}

// save pred data
void savePredData(DriverPrediction::PredData predData, Route* predRoute, City* city, bool close)
{
    static FILE* predRouteFile;
    static FILE* predDataFile;
    
    // route
    if(!predRouteFile)
    {
        predRouteFile = fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_PRED_ROUTE.csv", "w");
        fprintf(predRouteFile, "name, description, color, latitude, longitude\n");
    }
    
    saveRoute(predRoute, predRouteFile, city);
    
    // data
    if(!predDataFile)
    {
        predRouteFile = fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/DP_PRED_DATA.csv", "w");
    }
    
    fprintf(predDataFile, "--- spd n ele ---\n");
    
    for(int i = 0; i < predData.first.size(); i++)
    {
        fprintf(predDataFile, "%f,", predData.first.at(i));
    }
    
    fprintf(predDataFile, "\n");

    for(int i = 0; i < predData.second.size(); i++)
    {
        fprintf(predDataFile, "%f,", predData.second.at(i));
    }
    
    fprintf(predDataFile, "\n");
    
    if(close)
    {
        fclose(predRouteFile);
        fclose(predDataFile);
    }
}

int main() {
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                          Unit Tests
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
        city_ut();
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
    float travelDist;
    std::vector<float> spdBuf;
    
    // vehicle speed prediction distance between predictions
    float ds = SpeedPrediction().getDS();
    
    // ensure heading is along road spline trajectory
    bool headingIsStart2End = true;
    
    // containers for actual fuel and speed
    std::vector<float> actualSpeed;
    std::vector<float> fuelFlow;
    std::vector<float> energy;
    
    // ************************************************************************************************************************************
    // ******************************************************************v*****************************************************************
    //                                                      Run Program
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
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

        std::pair<double, double> prevLatLon = gps.readGPS();
        
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
                    currLink = Link().linkFromRoad(currRoad, currRoad->getEndIntersection());
                }
                else
                {
                    currLink = Link().linkFromRoad(currRoad, currRoad->getStartIntersection());
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
                    savePredData(predData, dp.getRP()->getPredictedRoute(), city, false);
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
                travelDist = gps.deltaLatLonToXY(prevLatLon.first, prevLatLon.second, currLatLon.first, currLatLon.second);
                
                float distRatio = travelDist / ds;
                
                if(distRatio >= 1)
                {
                    // buffer speed values is distance travel is greater than prediction interval distance
                    if(distRatio >= 2)
                    {
                        for(int i = 1; i <= distRatio; i++)
                        {
                            actualSpeed.push_back(vehSpd);
                            spdBuf.push_back(vehSpd);
                        }
                        dp.updateSpeedsByVec(&spdBuf);
                    }
                    
                    std::cout << "prediction distance: " << travelDist << std::endl;
                    std::cout << "*******************************************" << std::endl;
                    prevLatLon = currLatLon;
                    break;
                }
            }
        }
    }
    
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    //                                                  Store Data & Shutdown
    // ************************************************************************************************************************************
    // ************************************************************************************************************************************
    
    // create build city module to add new data if needed
    BuildCity bc;
    
    // get and store trip data
    dm.addTripData(gps.getTripLog(true));
    
    // update city data
    city = bc.getUpdatedCity();
    
    // store city data
    dm.addCityData(city);

    // get route from city using gps trace
    Route* actualRoute = city->getRouteFromGPSTrace(dm.getMostRecentTripData());
    
    // parse route
    dp.parseRoute(actualRoute, &actualSpeed);
    
    // store route prediction data
    dm.addRoutePredictionData(dp.getRP());
    
    // save actual route
    saveActualData(actualRoute, &actualSpeed, &fuelFlow, &energy, city);

	std::cout << "finished driver prediction" << std::endl;

	return 0;
}
