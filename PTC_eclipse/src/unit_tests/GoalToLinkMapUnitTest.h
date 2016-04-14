/*
 * GoalToLinkMapUnitTest.h
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#include "../route_prediction/GoalToLinkMap.h"
#include "../map/GenericMap.h"
#include "../route_prediction/GoalMapEntry.h"
#include "../driver_prediction/Link.h"
#include "../route_prediction/Goal.h"
#include <iostream>
#include <assert.h>

using namespace std;
using namespace PredictivePowertrain;
class GoalToLinkMapUnitTest {

Link* makeLinks(int numberOfLinks);
void goalToLinkMap_UT();

#endif /* UNIT_TESTS_GOALTOLINKMAPUNITTEST_H_ */
