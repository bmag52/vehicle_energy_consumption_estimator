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
    // TODO
    return NULL;
}

void routePrediction_UT()
{

}
