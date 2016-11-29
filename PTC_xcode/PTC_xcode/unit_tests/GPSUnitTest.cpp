/*
 * GPSUnitTest.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../gps/GPS.h"
#include "../data_management/DataManagement.h"
#include "../city/City.h"

using namespace PredictivePowertrain;

void GPS_ut() {
    
    DataManagement dm;
    City* city = dm.getCityData();
    
    GPS test;
    
    // heading
    int j = 0;
    while(j < 100)
    {
        Road* currRoad = test.getCurrentRoad1(city);
        
        std::cout << currRoad->getRoadID() << ": ";
        std::cout << test.isHeadingStart2EndOfCurrentRoad(currRoad);
        std::cout << std::endl;
        
        j++;
    }
    
    
    // ability to record gps measurements
    int i = 0;
    while(i < 10)
    {
        std::pair<double, double> latLon = test.readGPS();
        printf("%.6f,%.6f\n", latLon.first, latLon.second);
        i++;
    }
    
}
