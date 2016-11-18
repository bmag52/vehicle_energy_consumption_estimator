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

void saveRoute_ut(Route* route, FILE* file, City* city)
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

void city_ut()
{
    DataManagement dm;
    
    City* city = dm.getCityData();
    city->printIntersectionsAndRoads();
    Route* route = city->getRouteFromGPSTrace(dm.getMostRecentTripData());
    
    FILE* csvRoute = std::fopen("/Users/Brian/Desktop/the_goods/git/predictive_thermo_controller/data/CITY_TRACE_2_ROUTE.csv", "w");
    
    fprintf(csvRoute, "name, description, color, latitude, longitude\n");
    
    saveRoute_ut(route, csvRoute, city);
    
}
