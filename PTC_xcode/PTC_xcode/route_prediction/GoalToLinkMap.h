/*
 * GoalToLinkMap.h
 *
 *  Created on: Mar 13, 2016
 *      Author: vagrant
 */

#ifndef ROUTE_PREDICTION_GOALTOLINKMAP_H_
#define ROUTE_PREDICTION_GOALTOLINKMAP_H_
#include "../map/GenericMap.h"
#include "GoalMapEntry.h"
#include "../driver_prediction/Link.h"
#include "Goal.h"
#include <vector>



namespace PredictivePowertrain {

class GoalToLinkMap {
private:
	GenericMap<long int, GoalMapEntry<long int, int>*>* goalMap;
    
public:
	int maps_size;
	GoalToLinkMap();
	GoalToLinkMap(GoalToLinkMap &other);
	int linkTraversed(Link* link, Goal* goal);
    GenericMap<long int, GoalMapEntry<long int, int>*>* getGoalMap();
	std::vector<std::vector<float>*>* probabilityOfGoalsGivenLink(Link* link, Goal* goal, bool isSimilar);
	float probabilityOfGoalGivenLink(Link* link, Goal* goal, bool isSimilar);
	GenericMap<float, float> * probabilityOfGoalsGivenLinkMap(Link * link, Goal * goal, bool isSimilar);
	virtual ~GoalToLinkMap();
};

} /* namespace PredictivePowertrain */

#endif /* ROUTE_PREDICTION_GOALTOLINKMAP_H_ */
