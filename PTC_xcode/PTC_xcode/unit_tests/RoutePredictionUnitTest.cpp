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
    RoutePrediction pred(&city);
    Link link = links[0];
    int routeLength = 10;
    Intersection* startIntersection = city.getIntersectionFromLink(&link, false);
    
    // generate random actual route
    Route singleLinkRoute;
    singleLinkRoute.addLink(&link);
    singleLinkRoute.assignGoal(&goal);
    Route* route = city.randomPath(startIntersection, &singleLinkRoute, routeLength, conditions);
    
    // add trainging iterations here (simulates driving over the route multiple times)
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    
//    for(int i = 1; i <= 4; i++)
//    {
//        Route* randomRoute = city.randomPath(startIntersection, &singleLinkRoute, std::ceil(std::rand() / RAND_MAX * routeLength), conditions);
//        while(randomRoute.isequal(route))
//            randomRoute = city.randomPath(startIntersection, &singleLinkRoute, std::ceil(std::rand() / RAND_MAX * routeLength), conditions);
//        end
//        pred.parse_route(randomRoute);
//    }
//    
//    %% plot predicted route and update actual route
//    pred_route = pred.start_prediction(start_intersection,goal.bins);
//    for i=1:(length(route.links)-1)
//        pred_route = pred.predict_route(route.links{1});
//    
//        % plot predicted route
//        pred_route_handles = PlotRoute(fig, city, pred_route, 'g', 3);
//        
//        % show figure
//        xlabel('x coordinates');
//        ylabel('y coordinates');
//        title('Hidden Markov Model Visuals');
//        pause(0.1);
//        
//        % delete wrong route
//        if(~route.isequal(pred_route) == 1)
//            for ii = 1:length(pred_route_handles)
//                delete(pred_route_handles{ii});
//            end
//        end
//    
//        % update current route
//        route.links(1) = [];
//        set(actual_route_handles{i}, 'color', 'yellow');
//    end

}
