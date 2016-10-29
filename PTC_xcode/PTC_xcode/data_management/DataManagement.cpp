/*
 * DataManagement.cpp
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#include "DataManagement.h"

using boost::property_tree::ptree;
using boost::lexical_cast;

namespace PredictivePowertrain {

DataManagement::DataManagement() {
	std::string fileRay[3] = {this->routePredictionData, this->cityData, this->tripData};
	for(int i = 0; i < 3; i++)
	{
		std::ifstream dataFile(fileRay[i].c_str());
		if(!dataFile.good())
		{
			std::ofstream newFile(fileRay[i].c_str());
			newFile.close();
		}
		dataFile.close();
	}
}

int DataManagement::countFileLine(std::string fileLoc)
{
    int numLines = 0;
    std::string line;
    std::ifstream file(fileLoc.c_str());
    
    if(file.is_open()){
        while(!file.eof())
        {
            std::getline(file,line);
            numLines++;
        }
        file.close();
    }
    
    return numLines;
}

void DataManagement::addRoutePredictionData(RoutePrediction* rp)
{
    // links
    ptree links_ptree;
    rp->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = rp->getLinks()->nextEntry();
    while(nextLink != NULL)
    {
        ptree link_ptree;
        
        // print NN weights and activations
        int numNNLayers = nextLink->value->getNumNNLayers();
        
        if(numNNLayers > 0)
        {
            // get NN data
            std::vector<std::vector<Eigen::MatrixXd*>*>* nnAData = nextLink->value->getWeights(1);
            std::vector<std::vector<Eigen::MatrixXd*>*>* nnBData = nextLink->value->getWeights(0);
            
            for(int i = 0; i < nnAData->size(); i++)
            {
                ptree typeA_ptree, typeB_ptree;
                
                typeA_ptree.put("SIZE", nnAData->at(i)->size());
                typeB_ptree.put("SIZE", nnBData->at(i)->size());
                
                for(int j = 0; j < nnAData->at(i)->size(); j++)
                {
                    ptree typeAi_ptree, typeBi_ptree;
                    
                    typeAi_ptree.put("ROWS", nnAData->at(i)->at(j)->rows());
                    typeAi_ptree.put("COLS", nnAData->at(i)->at(j)->cols());
                    
                    typeBi_ptree.put("ROWS", nnBData->at(i)->at(j)->rows());
                    typeBi_ptree.put("COLS", nnBData->at(i)->at(j)->cols());
                    
                    for(int row = 0; row < nnAData->at(i)->at(j)->rows(); row++)
                    {
                        ptree typeAiRow_ptree, typeBiRow_ptree;
                        
                        for(int col = 0; col < nnAData->at(i)->at(j)->cols(); col++)
                        {
                            
                            typeAiRow_ptree.put(lexical_cast<std::string>(col), nnAData->at(i)->at(j)->coeffRef(row, col));
                            typeBiRow_ptree.put(lexical_cast<std::string>(col), nnBData->at(i)->at(j)->coeffRef(row, col));
                            
                        }
                        
                        typeAi_ptree.push_back(std::make_pair(lexical_cast<std::string>(row), typeAiRow_ptree));
                        typeBi_ptree.push_back(std::make_pair(lexical_cast<std::string>(row), typeBiRow_ptree));
                        
                    }
                    
                    typeA_ptree.push_back(std::make_pair(lexical_cast<std::string>(j), typeAi_ptree));
                    typeB_ptree.push_back(std::make_pair(lexical_cast<std::string>(j), typeBi_ptree));
                    
                }
                
                std::string type = "wts";
                if(i == 1)
                {
                    type = "yHid";
                }
                
                else if(i == 2)
                {
                    type = "yInHid";
                }
                
                link_ptree.push_back(std::make_pair(type + "A", typeA_ptree));
                link_ptree.push_back(std::make_pair(type + "B", typeB_ptree));
                
            }
            
        }
        
        link_ptree.put("DIRECTION", nextLink->value->getDirection());
        links_ptree.push_back(std::make_pair(lexical_cast<std::string>(nextLink->value->getNumber()),link_ptree));
        
        nextLink = rp->getLinks()->nextEntry();
    }
    delete(nextLink);
    
    // goals
    ptree goals_ptree;
    rp->getGoals()->initializeCounter();
    GenericEntry<long int, Goal*>* nextGoal = rp->getGoals()->nextEntry();
    while(nextGoal != NULL)
    {
        ptree goal_ptree, bins_ptree, numSeen_ptree;
        
        // add conditions
        for(int i = 0; i < nextGoal->value->getBins()->size(); i++)
        {
            ptree bin_ptree;
            bin_ptree.put("", nextGoal->value->getBins()->at(i));
            bins_ptree.push_back(std::make_pair(boost::lexical_cast<std::string>(i), bin_ptree));
        }
        
        // add num seen
        numSeen_ptree.put("", nextGoal->value->getNumSeen());
        
        // add conditions
        goal_ptree.push_back(std::make_pair("conditions", bins_ptree));
        
        // add number of times goal is seen
        goal_ptree.push_back(std::make_pair("numSeen", numSeen_ptree));
        
        // add goal to tree of goal
        goals_ptree.push_back(std::make_pair(boost::lexical_cast<std::string>(nextGoal->value->getDestination()), goal_ptree));
        
        nextGoal = rp->getGoals()->nextEntry();
    }
    delete(nextGoal);
    
    // link to state map
    ptree linkToStateMap_ptree;
    
    rp->getLinkToState()->getGoalMap()->initializeCounter();
    GenericEntry<long int, GoalMapEntry<long int, LinkToStateMapEntry*>*>* nextGoalMapEntryL2S = rp->getLinkToState()->getGoalMap()->nextEntry();
    while(nextGoalMapEntryL2S != NULL)
    {
        ptree nextGoalMapEntryL2S_ptree;
        
        nextGoalMapEntryL2S->value->getMap()->initializeCounter();
        GenericEntry<long int, LinkToStateMapEntry*>* nextLinkToStateMap = nextGoalMapEntryL2S->value->getMap()->nextEntry();
        while(nextLinkToStateMap != NULL)      {
            ptree nextLinkToStateMap_ptree;
            
            nextLinkToStateMap->value->getEntries()->initializeCounter();
            GenericEntry<long int, int>* nextLinkToStateEntry = nextLinkToStateMap->value->getEntries()->nextEntry();
            while(nextLinkToStateEntry != NULL)
            {
                ptree nextLinkToStateMapEntry_ptree;
                
                nextLinkToStateMapEntry_ptree.put("", nextLinkToStateEntry->value);
                
                // add entry to linkToStateMap Entry
                nextLinkToStateMap_ptree.push_back(std::make_pair(lexical_cast<std::string>(nextLinkToStateEntry->key), nextLinkToStateMapEntry_ptree));
                
                nextLinkToStateEntry = nextLinkToStateMap->value->getEntries()->nextEntry();
            }
            delete(nextLinkToStateEntry);
            
            // add linkToStateMap to goalMapEntry
            nextGoalMapEntryL2S_ptree.push_back(std::make_pair(lexical_cast<std::string>(nextLinkToStateMap->key), nextLinkToStateMap_ptree));
            
            nextLinkToStateMap = nextGoalMapEntryL2S->value->getMap()->nextEntry();
        }
        delete(nextLinkToStateMap);
        
        // add next goal map entry to link to state map
        linkToStateMap_ptree.push_back(std::make_pair(boost::lexical_cast<std::string>(nextGoalMapEntryL2S->key), nextGoalMapEntryL2S_ptree));
        
        nextGoalMapEntryL2S = rp->getLinkToState()->getGoalMap()->nextEntry();
    }
    delete(nextGoalMapEntryL2S);
    
    // goal to link map
    ptree goalToLinkMap_ptree;
    
    rp->getGoalToLink()->getGoalMap()->initializeCounter();
    GenericEntry<long int, GoalMapEntry<long int, int>*>* nextGoalMapEntryG2L = rp->getGoalToLink()->getGoalMap()->nextEntry();
    while(nextGoalMapEntryG2L != NULL)
    {
        ptree nextGoalMapEntryG2L_ptree;
        
        nextGoalMapEntryG2L->value->getMap()->initializeCounter();
        GenericEntry<long int, int>* nextGoalToLinkEntry = nextGoalMapEntryG2L->value->getMap()->nextEntry();
        while(nextGoalToLinkEntry != NULL)
        {
            ptree nextGoalToLinkEntry_ptree;
            
            nextGoalToLinkEntry_ptree.put("", nextGoalToLinkEntry->value);
            
            // add goal to link association to goal to link map
            nextGoalMapEntryG2L_ptree.push_back(std::make_pair(lexical_cast<std::string>(nextGoalToLinkEntry->key), nextGoalToLinkEntry_ptree));
            
            nextGoalToLinkEntry = nextGoalMapEntryG2L->value->getMap()->nextEntry();
        }
        delete(nextGoalToLinkEntry);
        
        // add goal to link map entry to tree of all map entries
        goalToLinkMap_ptree.push_back(std::make_pair(boost::lexical_cast<std::string>(nextGoalMapEntryG2L->key), nextGoalMapEntryG2L_ptree));
        
        nextGoalMapEntryG2L = rp->getGoalToLink()->getGoalMap()->nextEntry();
    }
    delete(nextGoalMapEntryG2L);
    
    // add all tree to route prediction json tree
    ptree rp_ptree;
    rp_ptree.push_back(std::make_pair("LINKS", links_ptree));
    rp_ptree.push_back(std::make_pair("GOALS", goals_ptree));
    rp_ptree.push_back(std::make_pair("LINK2STATE", linkToStateMap_ptree));
    rp_ptree.push_back(std::make_pair("GOAL2LINK", goalToLinkMap_ptree));
    
    write_json(this->routePredictionData, rp_ptree);
    
}

void DataManagement::addCityData(City* city)
{
	GenericMap<long int, Road*>* roadMap = city->getRoads();
	GenericMap<long int, Intersection*>* intersectionMap = city->getIntersections();
	GenericMap<int, Bounds*>* boundsMap = city->getBoundsMap();
	GenericMap<int, Bounds*>* newBoundsMap = new GenericMap<int, Bounds*>();

    ptree cityLogs;
	bool newBounds = false;
    if(this->countFileLine(this->cityData) == 0)
    {
        newBounds = true;
        delete(newBoundsMap);
        newBoundsMap = boundsMap;
    }
    else
    {
        try {
            read_json(this->cityData, cityLogs);

            boundsMap->initializeCounter();
            GenericEntry<int, Bounds*>* nextBounds = boundsMap->nextEntry();
            while(nextBounds != NULL)
            {
                bool boundsLogged = false;
                BOOST_FOREACH(ptree::value_type& v, cityLogs)
                {
                    int boundsID = lexical_cast<int>(v.first.data());
                    if(boundsID == nextBounds->key)
                    {
                        boundsLogged = true;
                        break;
                    }
                }
                if(!boundsLogged) {
                    newBounds = true;
                    newBoundsMap->addEntry(nextBounds->key, NULL);
                }
                nextBounds = boundsMap->nextEntry();
            }
        } catch(std::exception& e) {
            std::cout << e.what() << std::endl;
            newBoundsMap = boundsMap;
            newBounds = true;
        }
    }
	if(newBounds)
	{
        // add road data
		newBoundsMap->initializeCounter();
		GenericEntry<int, Bounds*>* nextBounds = newBoundsMap->nextEntry();
		while(nextBounds != NULL)
		{
			ptree boundsData, roads_ptree, intersections_ptree;
			roadMap->initializeCounter();
			GenericEntry<long int, Road*>* nextRoad = roadMap->nextEntry();
			while(nextRoad != NULL)
			{
				if(nextRoad->value->getBoundsID() == nextBounds->value->getID())
				{
					ptree road, startNode, endNode, roadType, nodes, splineLength;
					startNode.put("", nextRoad->value->getStartIntersection()->getIntersectionID());
					endNode.put("", nextRoad->value->getEndIntersection()->getIntersectionID());
					roadType.put("", nextRoad->value->getRoadType());
                    splineLength.put("", nextRoad->value->getSplineLength());

					ptree lats, lons, nodeElevations, nodeIDs;
					nextRoad->value->getNodes()->initializeCounter();
					GenericEntry<long int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
					while(nextNode != NULL)
					{
						Node* node = nextNode->value;
						ptree lat, lon, ele, id;
						lat.put("", node->getLat());
						lon.put("", node->getLon());
						ele.put("", node->getEle());
						id.put("", node->getID());

						lats.push_back(std::make_pair("", lat));
						lons.push_back(std::make_pair("", lon));
						nodeElevations.push_back(std::make_pair("", ele));
						nodeIDs.push_back(std::make_pair("", id));

						nextNode = nextRoad->value->getNodes()->nextEntry();
					}
					nodes.push_back(std::make_pair("latitude", lats));
					nodes.push_back(std::make_pair("longitude", lons));
					nodes.push_back(std::make_pair("elevation", nodeElevations));
					nodes.push_back(std::make_pair("nodeIDs", nodeIDs));

					road.push_back(std::make_pair("startNodeID", startNode));
					road.push_back(std::make_pair("endNodeID", endNode));
					road.push_back(std::make_pair("roadType", roadType));
                    road.push_back(std::make_pair("splineLength", splineLength));
					road.push_back(std::make_pair("nodes", nodes));

					roads_ptree.add_child(lexical_cast<std::string>(nextRoad->value->getRoadID()), road);
				}
				nextRoad = roadMap->nextEntry();
			}
            delete(nextRoad);

            // add intersecion data
			intersectionMap->initializeCounter();
			GenericEntry<long int, Intersection*>* nextIntersection = intersectionMap->nextEntry();
			while(nextIntersection != NULL)
			{
				if(nextIntersection->value->getBoudsID() == nextBounds->key)
				{
					ptree intersection, roadCount, elevation, lat, lon;
					roadCount.put("", nextIntersection->value->getRoadCount());
					elevation.put("", nextIntersection->value->getElevation());
					lat.put("", nextIntersection->value->getLat());
					lon.put("", nextIntersection->value->getLon());

					ptree roadIDs;
					GenericMap<long int, Road*>* connectingRoads = nextIntersection->value->getRoads();
					connectingRoads->initializeCounter();
					GenericEntry<long int, Road*>* nextRoad = connectingRoads->nextEntry();
					while(nextRoad != NULL)
					{
						ptree roadID;
						roadID.put("", nextRoad->value->getRoadID());
						roadIDs.push_back(std::make_pair("", roadID));
						nextRoad = connectingRoads->nextEntry();
					}

					intersection.push_back(std::make_pair("roadCount", roadCount));
					intersection.push_back(std::make_pair("elevation", elevation));
					intersection.push_back(std::make_pair("latitude", lat));
					intersection.push_back(std::make_pair("longitude", lon));
					intersection.push_back(std::make_pair("roadIDs", roadIDs));

					intersections_ptree.add_child(lexical_cast<std::string>(nextIntersection->value->getIntersectionID()), intersection);
				}
				nextIntersection = intersectionMap->nextEntry();
			}
            delete(nextIntersection);

			ptree bounds, maxLat, maxLon, minLat, minLon;
			maxLat.put("", boundsMap->getEntry(nextBounds->key)->getMaxLat());
			maxLon.put("", boundsMap->getEntry(nextBounds->key)->getMaxLon());
			minLat.put("", boundsMap->getEntry(nextBounds->key)->getMinLat());
			minLon.put("", boundsMap->getEntry(nextBounds->key)->getMinLon());

			bounds.push_back(std::make_pair("maxLat", maxLat));
			bounds.push_back(std::make_pair("maxLon", maxLon));
			bounds.push_back(std::make_pair("minLat", minLat));
			bounds.push_back(std::make_pair("minLon", minLon));

			boundsData.push_back(std::make_pair("roads", roads_ptree));
			boundsData.push_back(std::make_pair("intersections", intersections_ptree));
			boundsData.push_back(std::make_pair("bounds", bounds));

			cityLogs.add_child(lexical_cast<std::string>(nextBounds->key), boundsData);
			nextBounds = newBoundsMap->nextEntry();
		}
		write_json(this->cityData, cityLogs);
	}
}

void DataManagement::addTripData(GenericMap<long int, std::pair<double, double>*>* latLon) {

	int tripID = 0;
	ptree tripLogs;

    int numLines = this->countFileLine(this->tripData);
    if(numLines > 1)
    {
        // check for existing trips
        try
        {
            read_json(this->tripData, tripLogs);
            BOOST_FOREACH(ptree::value_type &v, tripLogs)
            {
                tripID = lexical_cast<int>(v.first.data());
            }
        }
        catch(const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    
	// add trip
	ptree trip, allLat, allLon;
	latLon->initializeCounter();
	GenericEntry<long int, std::pair<double, double>*>* nextLatLon = latLon->nextEntry();
	while(nextLatLon != NULL)
	{
		ptree lat, lon;
		lat.put("", nextLatLon->value->first);
		lon.put("", nextLatLon->value->second);

		allLat.push_back(std::make_pair("", lat));
		allLon.push_back(std::make_pair("", lon));
		nextLatLon = latLon->nextEntry();
	}

	trip.push_back(std::make_pair("latitude", allLat));
	trip.push_back(std::make_pair("longitude", allLon));

	tripLogs.add_child(lexical_cast<std::string>(tripID+1), trip);

	write_json(this->tripData, tripLogs);
}


RoutePrediction* DataManagement::getRoutePredictionData()
{
	try
    {
        
        ptree rpLogs;
        GenericMap<long int, Link*>* links = new GenericMap<long int, Link*>();
        GenericMap<long int, Goal*>* goals = new GenericMap<long int, Goal*>();
        LinkToStateMap* linkToState = new LinkToStateMap();
        GoalToLinkMap* goalToLink = new GoalToLinkMap();
        
		read_json(this->routePredictionData, rpLogs);
		BOOST_FOREACH(ptree::value_type &v, rpLogs)
		{
            std::string rpData = v.first.data();
            
            // LINKS
            if(!rpData.compare("LINKS"))
            {
                BOOST_FOREACH(ptree::value_type &u, v.second)
                {
                    long int linkNumber = lexical_cast<long int>(u.first.data());
                    int linkDirection;
                    
                    std::vector<Eigen::MatrixXd*>* wtsA;
                    std::vector<Eigen::MatrixXd*>* wtsB;
                    std::vector<Eigen::MatrixXd*>* yHidA;
                    std::vector<Eigen::MatrixXd*>* yHidB;
                    std::vector<Eigen::MatrixXd*>* yInHidA;
                    std::vector<Eigen::MatrixXd*>* yInHidB;
                    
                    BOOST_FOREACH(ptree::value_type &s, u.second)
                    {
                        std::string linkDataType = s.first.data();
                        
                        // read direction
                        if(!linkDataType.compare("DIRECTION"))
                        {
                            linkDirection = lexical_cast<int>(s.second.data());
                        }
                        
                        // read one of the NN matrices
                        else
                        {
                            // get matrix array size
                            int matRaySize;
                            BOOST_FOREACH(ptree::value_type &t, s.second)
                            {
                                std::string matFeatureRayType = t.first.data();
                                
                                if(!matFeatureRayType.compare("SIZE"))
                                {
                                    matRaySize = lexical_cast<int>(t.second.data());
                                    break;
                                }
                            }
                            
                            // iterate through matrix array to
                            std::vector<Eigen::MatrixXd*>* newMatRay = new std::vector<Eigen::MatrixXd*>(matRaySize);
                            BOOST_FOREACH(ptree::value_type &t, s.second)
                            {
                                std::string matFeatureRayType = t.first.data();
                                
                                if(matFeatureRayType.compare("SIZE"))
                                {
                                    int matRayIndex = lexical_cast<int>(matFeatureRayType);
                                    int rows = -1;
                                    int cols = -1;
                                    
                                    // iterate through matrix data to get size
                                    BOOST_FOREACH(ptree::value_type &x, t.second)
                                    {
                                        std::string matFeatureType = x.first.data();
                                        if(!matFeatureType.compare("ROWS"))
                                        {
                                            rows = lexical_cast<int>(x.second.data());
                                        }

                                        else if(!matFeatureType.compare("COLS"))
                                        {
                                            cols = lexical_cast<int>(x.second.data());
                                        }
                                        
                                        if(rows != -1 && cols != -1)
                                        {
                                            break;
                                        }
                                    }
                                    
                                    // iterate thorugh matrix to populate data
                                    newMatRay->at(matRayIndex) = new Eigen::MatrixXd(rows, cols);
                                    BOOST_FOREACH(ptree::value_type &x, t.second)
                                    {
                                        std::string matFeatureType = x.first.data();
                                        if(matFeatureType.compare("ROWS") && matFeatureType.compare("COLS"))
                                        {
                                            int row = lexical_cast<int>(matFeatureType);
                                            
                                            // iterate along matrix row
                                            BOOST_FOREACH(ptree::value_type &y, x.second)
                                            {
                                                int col = lexical_cast<int>(y.first.data());
                                                newMatRay->at(matRayIndex)->coeffRef(row, col) = lexical_cast<double>(y.second.data());
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if(!linkDataType.compare("wtsA"))
                            {
                                wtsA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("wtsB"))
                            {
                                wtsB = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yHidA"))
                            {
                                yHidA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yHidB"))
                            {
                                yHidB = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yInHidA"))
                            {
                                yInHidA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yInHidB"))
                            {
                                yInHidB = newMatRay;
                            }
                        }
                    }
                    
                    Link* newLink = new Link(linkDirection, linkNumber);
                    newLink->setWeights(wtsA, yHidA, yInHidA, 1);
                    newLink->setWeights(wtsB, yHidB, yInHidB, 0);
                    newLink->setNumNNLayers((int)wtsA->size());
                    
                    links->addEntry(newLink->getHash(), newLink);
                }
            }
            
            // GOALS
            else if(!rpData.compare("GOALS"))
            {
                BOOST_FOREACH(ptree::value_type &u, v.second)
                {
                    long int destinationNum = lexical_cast<long int>(u.first.data());
                    std::vector<float>* conditionsVec;
                    int numSeen;
                    
                    // get conditions and numseen
                    BOOST_FOREACH(ptree::value_type &t, u.second)
                    {
                        std::string goalFeature = t.first.data();
                        
                        if(!goalFeature.compare("conditions"))
                        {
                            // get conditions from json
                            GenericMap<int, float> conditionsMap;
                            BOOST_FOREACH(ptree::value_type &s, t.second)
                            {
                                int index = lexical_cast<int>(s.first.data());
                                float condition = lexical_cast<float>(s.second.data());
                                
                                conditionsMap.addEntry(index, condition);
                            }
                            
                            // add conditions to vector
                            conditionsVec = new std::vector<float>(conditionsMap.getSize());
                            
                            conditionsMap.initializeCounter();
                            GenericEntry<int, float>* nextCondition = conditionsMap.nextEntry();
                            while(nextCondition != NULL)
                            {
                                conditionsVec->at(nextCondition->key) = nextCondition->value;
                                nextCondition = conditionsMap.nextEntry();
                            }
                            delete(nextCondition);
                        } else if(!goalFeature.compare("numSeen"))
                        {
                            numSeen = lexical_cast<int>(t.second.data());
                        }
                    }
                    
                    Goal* newGoal = new Goal(destinationNum, conditionsVec);
                    newGoal->setNumSeen(numSeen);
                    
                    goals->addEntry(newGoal->getHash(), newGoal);
                }
            }
            
            // LINK2STATE
            else if(!rpData.compare("LINK2STATE"))
            {
                BOOST_FOREACH(ptree::value_type &u, v.second)
                {
                    // iterate through each goal map entry
                    long int goalHash = lexical_cast<long int>(u.first.data());
                    assert(goals->hasEntry(goalHash));
                    
                    BOOST_FOREACH(ptree::value_type &s, u.second)
                    {
                        // grab all link assocations for a particular goal
                        long int fromLinkHash = lexical_cast<long int>(s.first.data());
                        assert(links->hasEntry(fromLinkHash));
                        
                        GenericMap<long int, int> toLinkAssociations;
                        BOOST_FOREACH(ptree::value_type &t, s.second)
                        {
                            long int toLinkHash = lexical_cast<long int>(t.first.data());
                            int numAssociated = lexical_cast<int>(t.second.data());
                            toLinkAssociations.addEntry(toLinkHash, numAssociated);
                        }
                        
                        // populate link to state map using known associations
                        toLinkAssociations.initializeCounter();
                        GenericEntry<long int, int>* nextToLinkAssociation = toLinkAssociations.nextEntry();
                        while(nextToLinkAssociation != NULL)
                        {
                            assert(links->hasEntry(nextToLinkAssociation->key));
                            
                            Link* fromLink = links->getEntry(fromLinkHash);
                            Link* toLink = links->getEntry(nextToLinkAssociation->key);
                            Goal* toGoal = goals->getEntry(goalHash);
                            
                            // make number of associations specified
                            for(int i = 0; i < nextToLinkAssociation->value; i++)
                            {
                                linkToState->incrementTransition(fromLink, toGoal, toLink);
                            }
                            
                            nextToLinkAssociation = toLinkAssociations.nextEntry();
                        }
                        delete(nextToLinkAssociation);
                    }
                }
            }
            
            // GOAL2LINK
            else if(!rpData.compare("GOAL2LINK"))
            {
                BOOST_FOREACH(ptree::value_type &u, v.second)
                {
                    long int goalHash = lexical_cast<long int>(u.first.data());
                    assert(goals->hasEntry(goalHash));
                    
                    // grab all link assocations to given goal
                    BOOST_FOREACH(ptree::value_type &t, u.second)
                    {
                        long int linkHash = lexical_cast<long int>(t.first.data());
                        assert(links->hasEntry(linkHash));
                        
                        // impress assocations on goal2link map
                        int numAssociations = lexical_cast<int>(t.second.data());
                        for(int i = 0; i < numAssociations; i++)
                        {
                            Link* linki = links->getEntry(linkHash);
                            Goal* goali = goals->getEntry(goalHash);
                            
                            goalToLink->linkTraversed(linki, goali);
                        }
                    }
                }
            }
		}
        
        RoutePrediction* rp = new RoutePrediction();
        rp->addPredictionElements(links, goals, goalToLink, linkToState);
        
        return rp;
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
    
    return NULL;
}

City* DataManagement::getCityData()
{

    if(this->countFileLine(this->cityData) == 0)
    {
        return NULL;
    }
    
	ptree cityLogs;
	try
    {
		read_json(this->cityData, cityLogs);
		GenericMap<long int, Road*>* roads = new GenericMap<long int, Road*>();
		GenericMap<long int, Intersection*>* intersections = new GenericMap<long int, Intersection*>();
		GenericMap<int, Bounds*>* bounds = new GenericMap<int, Bounds*>();
		GenericMap<long int, std::pair<int, int>*>* roadIntersections = new GenericMap<long int, std::pair<int, int>*>(); // <roadID, <startID, endID>>

		// get roads first
		BOOST_FOREACH(ptree::value_type& u, cityLogs)
		{
			int boundsID = lexical_cast<int>(u.first.data());
			BOOST_FOREACH(ptree::value_type& v, u.second)
			{
				std::string child = v.first.data();
				if(!child.compare("roads")) {

					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						GenericMap<int, double>* nodeLats = new GenericMap<int, double>();
						GenericMap<int, double>* nodeLons = new GenericMap<int, double>();
						GenericMap<int, int>* nodeEles = new GenericMap<int, int>();
						GenericMap<int, long int>* nodeIDs = new GenericMap<int, long int>();

						long int roadID = lexical_cast<long int>(z.first.data());
						long int startNodeID, endNodeID;
                        float splineLength;
						std::string roadType;

						BOOST_FOREACH(ptree::value_type& a, z.second)
						{
							std::string roadFeature = a.first.data();
							if(!roadFeature.compare("startNodeID")) {
								startNodeID = lexical_cast<int>(a.second.data());
							} else if(!roadFeature.compare("endNodeID")) {
								endNodeID = lexical_cast<int>(a.second.data());
							} else if(!roadFeature.compare("roadType")) {
								roadType = a.second.data();
                            }else if(!roadFeature.compare("splineLength")) {
                                splineLength = lexical_cast<float>(a.second.data());
							} else if(!roadFeature.compare("nodes")) {
								int latCount = 0; int lonCount = 0; int eleCount = 0; int idCount = 0;
								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									std::string nodeFeature = b.first.data();
									BOOST_FOREACH(ptree::value_type& c, b.second)
									{
										if(!nodeFeature.compare("latitude")) {
											nodeLats->addEntry(latCount++, lexical_cast<double>(c.second.data()));
										} else if(!nodeFeature.compare("longitude")) {
											nodeLons->addEntry(lonCount++, lexical_cast<double>(c.second.data()));
										} else if(!nodeFeature.compare("elevation")) {
											nodeEles->addEntry(eleCount++, lexical_cast<int>(c.second.data()));
										} else if(!nodeFeature.compare("nodeIDs")) {
											nodeIDs->addEntry(idCount++, lexical_cast<long int>(c.second.data()));
										}
									}
								}
							}
						}

						GenericMap<long int, Node*>* nodes = new GenericMap<long int, Node*>();
						for(int i = 1; i <= nodeLats->getSize(); i++)
						{

							nodes->addEntry(i, new Node(nodeLats->getEntry(i), nodeLons->getEntry(i), nodeEles->getEntry(i), nodeIDs->getEntry(i)));
						}
						delete(nodeLats); delete(nodeLons); delete(nodeEles); delete(nodeIDs);

						roadIntersections->addEntry(roadID, new std::pair<int, int>(startNodeID, endNodeID));
                        
                        Road* road = new Road(roadType, roadID, nodes);
                        road->assignSplineLength(splineLength);
						roads->addEntry(roadID, road);
					}
				}
			}
		}

		// get intersections and bounds next adding roads to the intersections
		BOOST_FOREACH(ptree::value_type& u, cityLogs)
		{
			int boundsID = lexical_cast<int>(u.first.data());
			BOOST_FOREACH(ptree::value_type& v, u.second)
			{
				std::string child = v.first.data();
				if(!child.compare("intersections")) {
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						int intID = lexical_cast<int>(z.first.data());
						GenericMap<long int, Road*>* intRoads = new GenericMap<long int, Road*>();
						int ele; double lat, lon;
						BOOST_FOREACH(ptree::value_type& a, z.second)
						{
							std::string intFeature = a.first.data();
							if(!intFeature.compare("elevation")) {
								ele = lexical_cast<int>(a.second.data());
							} else if(!intFeature.compare("latitude")) {
								lat = lexical_cast<double>(a.second.data());
							} else if(!intFeature.compare("longitude")) {
								lon = lexical_cast<double>(a.second.data());
							} else if(!intFeature.compare("roadIDs")) {

								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									long int roadID = lexical_cast<long int>(b.second.data());
									intRoads->addEntry(roadID, roads->getEntry(roadID));
								}
							}
						}
						Intersection* newInt = new Intersection(intRoads, lat, lon, ele, intID);
						newInt->setBoundsID(boundsID);
						intersections->addEntry(intID, newInt);
					}
				} else if(!child.compare("bounds")) {
					double maxLat, maxLon, minLat, minLon;
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
						std::string boundsFeature = z.first.data();
						if(!boundsFeature.compare("maxLat")) {
							maxLat = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("maxLon")) {
							maxLon = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("minLat")) {
							minLat = lexical_cast<double>(z.second.data());
						} else if(!boundsFeature.compare("minLon")) {
							minLon = lexical_cast<double>(z.second.data());
						}
					}
					Bounds* newBounds = new Bounds(maxLat, maxLon, minLat, minLon);
					newBounds->assignID(boundsID);
					bounds->addEntry(boundsID, newBounds);
				}
			}
		}

		roadIntersections->initializeCounter();
		GenericEntry<long int, std::pair<int, int>*>* nextRoadInts = roadIntersections->nextEntry();
		while(nextRoadInts != NULL)
		{
			roads->getEntry(nextRoadInts->key)->setStartIntersection(intersections->getEntry(nextRoadInts->value->first));
			roads->getEntry(nextRoadInts->key)->setEndIntersection(intersections->getEntry(nextRoadInts->value->second));
			nextRoadInts = roadIntersections->nextEntry();
		}

		delete(roadIntersections);
		return new City(intersections, roads, bounds);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return NULL;
	}
}

GenericMap<long int, std::pair<double, double>*>* DataManagement::getMostRecentTripData() {
    
    std::cout << "getting most recent trip log data" << std::endl;
    
    if(this->countFileLine(this->tripData) == 0)
    {
        return NULL;
    }

	ptree tripLog;
	int dayID = 0;
	try {
		read_json(this->tripData, tripLog);
		BOOST_FOREACH(ptree::value_type& v, tripLog)
		{
			dayID = lexical_cast<int>(v.first.data());
		}

		GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>* rawRecentTripData = new GenericMap<GenericMap<int, double>*, GenericMap<int, double>*>();
		int latCount = 0, lonCount = 0;
		BOOST_FOREACH(ptree::value_type& v, tripLog)
		{
			if(lexical_cast<int>(v.first.data()) == dayID)
			{
				GenericMap<int, double>* lats = new GenericMap<int, double>();
				GenericMap<int, double>* lons = new GenericMap<int, double>();
				BOOST_FOREACH(ptree::value_type& z, v.second)
				{
					std::string subTree = z.first.data();

					BOOST_FOREACH(ptree::value_type& b, z.second)
					{
						if(!subTree.compare("latitude"))
						{
							double lat = lexical_cast<double>(b.second.data());
							lats->addEntry(latCount++, lat);
							std::cout << subTree << " = " << lat << std::endl;
						} else if(!subTree.compare("longitude")) {
							double lon = lexical_cast<double>(b.second.data());
							lons->addEntry(lonCount++, lon);
							std::cout << subTree << " = " << lon << std::endl;
						}
					}

				}
				assert(lats->getSize() == lons->getSize());
				rawRecentTripData->addEntry(lats, lons);
			}
		}

		GenericMap<long int, std::pair<double, double>*>* recentTripData = new GenericMap<long int, std::pair<double, double>*>();
		long int tripCount = 1;

		rawRecentTripData->initializeCounter();
		GenericEntry<GenericMap<int, double>*, GenericMap<int, double>*>* nextLatLonSet = rawRecentTripData->nextEntry();
		while(nextLatLonSet != NULL)
		{
			nextLatLonSet->key->initializeCounter();
			nextLatLonSet->value->initializeCounter();
			GenericEntry<int, double>* nextLat = nextLatLonSet->key->nextEntry();
			GenericEntry<int, double>* nextLon = nextLatLonSet->value->nextEntry();
			while(nextLat != NULL && nextLon != NULL)
			{
				recentTripData->addEntry(tripCount++, new std::pair<double, double>(nextLat->value, nextLon->value));
				nextLat = nextLatLonSet->key->nextEntry();
				nextLon = nextLatLonSet->value->nextEntry();
			}
			delete(nextLat);
			delete(nextLon);
			nextLatLonSet = rawRecentTripData->nextEntry();
		}
		delete(nextLatLonSet);
		delete(rawRecentTripData);

		return recentTripData;
	} catch(const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return NULL;
}

}





