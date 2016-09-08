/*
 * RoutePredictionUnitTest.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "../driver_prediction/Link.h"
#include "../city/City.h"
#include "../city/Intersection.h"
#include "../route_prediction/Goal.h"
#include "../data_management/DataManagement.h"
#include "../route_prediction/RoutePrediction.h"
#include "../route_prediction/Route.h"

#include <math.h>
#include <iostream>
#include <assert.h>
#include <climits>
#include <algorithm>
#include <ctime>

using namespace PredictivePowertrain;

Link* makeLinks(int numberOfLinks)
{
	Link* links = new Link[numberOfLinks];
	for(int i = 0; i < numberOfLinks; i++)
	{
		Link newLink(rand() > .5, i + 1);
		links[i] = newLink;
	}
    return links;
}

void routePrediction_UT()
{
    // seed random generator
    std::srand(std::time(0));
    
    // create a square city grid to predict over
    const int gridSideDim = 4;
    const int numInters = gridSideDim * gridSideDim;
    Intersection inters[numInters];
    
    GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
    GenericMap<int, Intersection*>* intersections = new GenericMap<int, Intersection*>();
    
    // fence post addage of the first intersection
    inters[0].assignID(1);
    intersections->addEntry(inters[0].getIntersectionID(), &inters[0]);
    
    // make all grid connections
    int horzCount = 0;
    int vertCount = (gridSideDim - 1) * gridSideDim;
    for(int i = 1; i < numInters; i++)
    {
        int interID = i + 1;
        
        inters[i].assignID(interID);
        
        // make horizontal connections
        if(i % gridSideDim != 0)
        {
            horzCount++;
            
            Road* newHorzRoad = new Road();
            newHorzRoad->assignID(horzCount);
            
            inters[i].addRoad(newHorzRoad, 1);
            inters[i-1].addRoad(newHorzRoad, 0);
            
            roads->addEntry(horzCount, newHorzRoad);
        }
        
        // make vertical conections
        if(interID > gridSideDim)
        {
            vertCount++;
            
            Road* newVertRoad = new Road();
            newVertRoad->assignID(vertCount);
            
            inters[i].addRoad(newVertRoad, 1);
            inters[i - gridSideDim].addRoad(newVertRoad, 0);
            
            roads->addEntry(vertCount, newVertRoad);
        }
        
        intersections->addEntry(inters[i].getIntersectionID(), &inters[i]);
    }
    
    // build city
    City city(intersections, roads, new GenericMap<int, Bounds*>());
    Link* links = makeLinks(5);
    int conditions[] = {1, 2};
    Goal goal(1, conditions);
    
    // add routes to route prediction
    RoutePrediction routePrediction(&city);
    Link link = links[0];
    int routeLength = 10;
    Intersection* startIntersection = city.getIntersectionFromLink(&link, false);
    
    // generate random actual route
    Route startRoute;
    startRoute.addLink(&link);
    startRoute.assignGoal(&goal);
    Route* route = city.randomPath(startIntersection, &startRoute, routeLength, conditions);
    
    std::cout << "143542612" << std::endl;
    std::cout <<  std::rand() << std::endl;
    std::cout << RAND_MAX << std::endl;
    std::cout << (double)std::rand() / RAND_MAX << std::endl;
    std::cout << (double)std::rand() / RAND_MAX * routeLength << std::endl;
    std::cout << std::ceil((double)std::rand() / RAND_MAX * routeLength) << std::endl;
    
    // add trainging iterations here (simulates driving over the route multiple times)
    routePrediction.parseRoute(route);
    routePrediction.parseRoute(route);
    routePrediction.parseRoute(route);
    routePrediction.parseRoute(route);
    routePrediction.parseRoute(route);
    
    // create number of random routes to include in test set
    int num_rand_routes = 4;
    
    for(int i = 1; i <= num_rand_routes; i++)
    {
        // create random route
        Route* randomRoute = city.randomPath(startIntersection, &startRoute, std::ceil((float)std::rand() / RAND_MAX * routeLength), conditions);
        while(randomRoute->isEqual(route))
        {
            randomRoute = city.randomPath(startIntersection, &startRoute, std::ceil((float)std::rand() / RAND_MAX * routeLength), conditions);
        }
        
        // add random route to test set
        routePrediction.parseRoute(randomRoute);
    }
    
    // predict actual route as it is 'driven' over
    Route* predRoute = routePrediction.startPrediction(startIntersection, conditions);
    for(int i = 1; i <= route->getLinkSize(); i++)
    {
        // predict route
        predRoute = routePrediction.predict(route->getLinks()->getEntry(0));
        
        // check if predicted route is actual route
        if(route->isEqual(predRoute))
        {
            std::cout << "predicted routed before reaching end destination!" << std::endl;
            std::cout << "Links traversed: " << i << endl;
            std::cout << "Links in route: " << route->getLinkSize() << std::endl;
            break;
        }
    
        // update actual route as it's 'driven' over
        route->removeFirstLink();
    }

}
