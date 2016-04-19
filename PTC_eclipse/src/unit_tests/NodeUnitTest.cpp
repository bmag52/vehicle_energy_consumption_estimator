/*
 * NodeUnitTest.cpp
 *
 *  Created on: Apr 14, 2016
 *      Author: vagrant
 */

#include "../data_collection/Node.h"
#include <iostream>
#include <assert.h>
#include "UnitTests.h"


using namespace PredictivePowertrain;

void node_ut(){

Node node1(0,0,0);
Node node2(1.0, 1.0, 1);
Node node3(15.5, 16.5, 20);

assert(node1.getLat() == 0);
assert(node1.getLon() == 0);
assert(node1.getID() == 0);

assert(node2.getLat() == 1.0);
assert(node2.getLon() == 1.0);
assert(node2.getID() == 1);

assert(node3.getLat() == 15.5);
assert(node3.getLon() == 16.5);
assert(node3.getID() == 20);

//missing: getEle(). Unsure what the expected value should be.

}



