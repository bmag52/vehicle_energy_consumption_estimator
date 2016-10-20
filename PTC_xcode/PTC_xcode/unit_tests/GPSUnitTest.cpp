/*
 * GPSUnitTest.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: vagrant
 */

#include "UnitTests.h"
#include "../gps/GPS.h"

using namespace PredictivePowertrain;

void GPS_ut() {
    
    GPS test;
    
    int i = 0;
    while(i < 10)
    {
        std::pair<double, double> latLon = test.readGPS();
        
        printf("%.6f,%.6f\n", latLon.first, latLon.second);
        
        i++;
    }
    
}
