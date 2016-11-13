/*
 * unit_tests.h
 *
 *  Created on: Mar 16, 2016
 *      Author: vagrant
 */

#ifndef UNIT_TESTS_UNITTESTS_H_
#define UNIT_TESTS_UNITTESTS_H_

#include "../route_prediction/RoutePrediction.h"
#include "../speed_prediction/SpeedPrediction.h"

void buildCity_ut();
void citySection_ut();
void dataCollection_ut();
void dataManagement_ut();
void elevationtypes_ut();
void goalmapentry_ut();
void goalToLinkMap_ut();
void link_ut();
void intersection_ut();
void intersectiontypes_ut();
void linkToStateMapEntry_ut();
void linkToStateMap_ut();
void node_ut();
void route_ut();
PredictivePowertrain::RoutePrediction* routePrediction_ut();
PredictivePowertrain::SpeedPrediction* speedPrediction_ut();
void roadTypes_ut();
void kinematics_ut();
void GPS_ut();
void driverPrediction_ut();
void vehicleDiagnostics_ut();
#endif /* UNIT_TESTS_UNITTESTS_H_ */
