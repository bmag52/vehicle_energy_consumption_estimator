/*
 * RoutePredictionUnitTest.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#include "../driver_prediction/Link.h"
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
}

void routePrediction_ut()
{
	std::cout << "Route Prediction Unit Test" << std::endl;

	int i = 0;
	while(i < 1000000)
	{
		int ii;
		while(ii < 1000000)
		{
			ii++;
		}
		i++;
	}
}
