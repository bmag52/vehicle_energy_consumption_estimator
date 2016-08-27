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
#include <iostream>
#include <assert.h>

using namespace PredictivePowertrain;

Link* makeLinks(int numberOfLinks)
{
	Link* links = new Link[numberOfLinks];
	for(int i = 0; i < numberOfLinks; i++)
	{
		Link newLink(i, rand() > .5);
		links[i] = newLink;
	}
    return links;
}

void routePrediction_UT()
{
    // for now get city data from data management unit tests
    DataManagement dm;
    
    // build city
    City* city = dm.getCityData();
    Link* links = makeLinks(5);
    int conditions[] = {1, 2};
    Goal goal(1, conditions);
    
    // add routes to route prediction
    RoutePrediction pred(city);
    Link link = links[0];
    int routeLength = 10;
    Intersection* startIntersection = city->getIntersectionFromLink(&link, false);
    
    // generate random actual route
    GenericMap<int, Link*> singleLinkMap;
    singleLinkMap.addEntry(1, &link);
    Route singleLinkRoute(&singleLinkMap, &goal);
    Route* route = city->randomPath(startIntersection, &singleLinkRoute, routeLength, conditions);
    
    // add trainging iterations here (simulates driving over the route multiple times)
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    pred.parseRoute(route);
    
//    % add 4 random routes
//    for i = 1:4
//        randomRoute = city.random_path(start_intersection,{link},ceil(rand*route_length),goal.bins);
//        while(randomRoute.isequal(route))
//            randomRoute = city.random_path(start_intersection,{link},ceil(rand*route_length),goal.bins);
//        end
//        pred.parse_route(randomRoute);
//    end
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
