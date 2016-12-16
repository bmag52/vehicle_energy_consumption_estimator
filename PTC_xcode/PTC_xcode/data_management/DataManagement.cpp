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
    std::cout << "updating route predicition data" << std::endl;
    // links
    ptree links_ptree;
    
    std::ofstream out(this->nnDataLoc.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    
    rp->getLinks()->initializeCounter();
    GenericEntry<long int, Link*>* nextLink = rp->getLinks()->nextEntry();
    while(nextLink != NULL)
    {
        ptree link_ptree;
        
        link_ptree.put("NUMBER", nextLink->value->getNumber());
        link_ptree.put("DIRECTION", nextLink->value->getDirection());
        
        // print NN weights and activations
        if(nextLink->value->linkHasWeights())
        {
            // get NN data
            std::vector<std::vector<std::vector<Eigen::MatrixXd*>*>*> nnData;
            std::vector<std::string> nnDataType;
            
            if(nextLink->value->linkHasAWeights())
            {
                nnData.push_back(nextLink->value->getWeights(1));
                nnDataType.push_back("A");
            }
            
            if(nextLink->value->linkHasBWeights())
            {
                nnData.push_back(nextLink->value->getWeights(0));
                nnDataType.push_back("B");
            }
            
            // write to json
            if(this->jsonifyLinkNNData)
            {
                for(int h = 0; h < nnData.size(); h++)
                {
                    for(int i = 0; i < nnData.at(h)->size(); i++)
                    {
                        ptree type_ptree;
                        
                        type_ptree.put("SIZE", nnData.at(h)->at(i)->size());
                        
                        for(int j = 0; j < nnData.at(h)->at(i)->size(); j++)
                        {
                            ptree type_i_ptree;
                            
                            type_i_ptree.put("ROWS", nnData.at(h)->at(i)->at(j)->rows());
                            type_i_ptree.put("COLS", nnData.at(h)->at(i)->at(j)->cols());
                            
                            for(int row = 0; row < nnData.at(h)->at(i)->at(j)->rows(); row++)
                            {
                                ptree type_i_row_ptree;
                                
                                for(int col = 0; col < nnData.at(h)->at(i)->at(j)->cols(); col++)
                                {
                            
                                    type_i_row_ptree.put(lexical_cast<std::string>(col), nnData.at(h)->at(i)->at(j)->coeffRef(row, col));
                                    
                                }
                                
                                type_i_ptree.push_back(std::make_pair(lexical_cast<std::string>(row), type_i_row_ptree));
                                
                            }
                            
                            type_ptree.push_back(std::make_pair(lexical_cast<std::string>(j), type_i_ptree));
                            
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
                        
                        link_ptree.push_back(std::make_pair(type + nnDataType.at(h), type_ptree));
                        
                    }
                }
            }
            
            // binary serialization
            else
            {
                bool addedLink = false;
                
                // type A or B
                for(int h = 0; h < nnData.size(); h++)
                {
                    // wts or yHidAct or yInAct
                    for(int i = 0; i < nnData.at(h)->size(); i++)
                    {
                        // layer
                        for(int j = 0; j < nnData.at(h)->at(i)->size(); j++)
                        {
                            Eigen::MatrixXd* nnMat = nnData.at(h)->at(i)->at(j);
                            
                            if(nnMat != NULL)
                            {
                                addedLink = true;
                                
                                // write it twice becuase last mat written sometimes doesnt get picked up in read
                                this->writeBinaryNNMat(out, *nnMat, i, j, nextLink->value->getHash());
                                this->writeBinaryNNMat(out, *nnMat, i, j, nextLink->value->getHash());
                            }
                        }
                    }
                }
                
                if(addedLink)
                {
                    std::cout << nextLink->value->getNumber() << std::endl;
                }
            }
        }
        
        links_ptree.push_back(std::make_pair(lexical_cast<std::string>(nextLink->value->getHash()),link_ptree));
        
        nextLink = rp->getLinks()->nextEntry();
    }
    delete(nextLink);
    out.close();
    
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
    std::cout << "updating city data" << std::endl;
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
            
            std::ofstream out(this->nodeDataLoc.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
            
			roadMap->initializeCounter();
			GenericEntry<long int, Road*>* nextRoad = roadMap->nextEntry();
			while(nextRoad != NULL)
			{
				if(nextRoad->value->getBoundsID() == nextBounds->value->getID())
				{
					ptree road, startNode, endNode, roadType, splineLength;
					startNode.put("", nextRoad->value->getStartIntersection()->getIntersectionID());
					endNode.put("", nextRoad->value->getEndIntersection()->getIntersectionID());
					roadType.put("", nextRoad->value->getRoadType());
                    splineLength.put("", nextRoad->value->getSplineLength());

                    if(this->jsonifyRoadNodes)
                    {
                        ptree lats, lons, nodeElevations, nodeIDs, nodes;
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
                        delete(nextNode);
                        
                        nodes.push_back(std::make_pair("latitude", lats));
                        nodes.push_back(std::make_pair("longitude", lons));
                        nodes.push_back(std::make_pair("elevation", nodeElevations));
                        nodes.push_back(std::make_pair("nodeIDs", nodeIDs));
                        road.push_back(std::make_pair("nodes", nodes));
                    }
                    else
                    {
                        Eigen::MatrixXd nodesMat(nextRoad->value->getNodes()->getSize(), 4);
                        int nodeRow = 0;
                        
                        nextRoad->value->getNodes()->initializeCounter();
                        GenericEntry<long int, Node*>* nextNode = nextRoad->value->getNodes()->nextEntry();
                        while(nextNode != NULL)
                        {
                            Node* node = nextNode->value;
                            
                            nodesMat.coeffRef(nodeRow, 0) = node->getLat();
                            nodesMat.coeffRef(nodeRow, 1) = node->getLon();
                            nodesMat.coeffRef(nodeRow, 2) = node->getEle();
                            nodesMat.coeffRef(nodeRow, 3) = node->getID();
                            nodeRow++;
                            
                            nextNode = nextRoad->value->getNodes()->nextEntry();
                        }
                        delete(nextNode);
                        
                        this->writeBinaryNodeMat(out, nodesMat, nextRoad->value->getRoadID());
                    }
                    
					road.push_back(std::make_pair("startNodeID", startNode));
					road.push_back(std::make_pair("endNodeID", endNode));
					road.push_back(std::make_pair("roadType", roadType));
                    road.push_back(std::make_pair("splineLength", splineLength));

					roads_ptree.add_child(lexical_cast<std::string>(nextRoad->value->getRoadID()), road);
				}
				nextRoad = roadMap->nextEntry();
			}
            delete(nextRoad);
            
            out.close();

            // add intersecion data
			intersectionMap->initializeCounter();
			GenericEntry<long int, Intersection*>* nextIntersection = intersectionMap->nextEntry();
			while(nextIntersection != NULL)
			{
				if(nextIntersection->value->getBoudsID() == nextBounds->value->getID())
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

void DataManagement::addTripData(GenericMap<long int, std::pair<double, double>*>* latLon)
{
    std::cout << "updating trip log" << std::endl;
    
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
    std::cout << "getting route prediciton data" << std::endl;
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
                bool hasANNVals = false;
                bool hasBNNVals = false;
                
                std::cout << "in get rp, links" << std::endl;
                BOOST_FOREACH(ptree::value_type &u, v.second)
                {
                    int linkDirection;
                    long int linkNumber;
                    long int linkHash = lexical_cast<long int>(u.first.data());
                    
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
                        // read link number
                        else if(!linkDataType.compare("NUMBER"))
                        {
                            linkNumber = lexical_cast<long int>(s.second.data());
                        }
                        // read one of the NN matrices
                        else if(this->jsonifyLinkNNData)
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
                                hasANNVals = true;
                                wtsA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("wtsB"))
                            {
                                hasBNNVals = true;
                                wtsB = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yHidA"))
                            {
                                hasANNVals = true;
                                yHidA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yHidB"))
                            {
                                hasBNNVals = true;
                                yHidB = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yInHidA"))
                            {
                                hasANNVals = true;
                                yInHidA = newMatRay;
                            }
                            
                            else if(!linkDataType.compare("yInHidB"))
                            {
                                hasBNNVals = true;
                                yInHidB = newMatRay;
                            }
                        }
                    }
                    
                    Link* newLink = new Link(linkDirection, linkNumber);
                    
                    if(hasANNVals)
                    {
                        newLink->setWeights(wtsA, yHidA, yInHidA, 1);
                    }
                    
                    if(hasBNNVals)
                    {
                        newLink->setWeights(wtsB, yHidB, yInHidB, 0);
                    }
                    
                    links->addEntry(linkHash, newLink);
                }
                
                // add final link because link to state include final link
                Link* finalLink = Link().finalLink();
                links->addEntry(finalLink->getHash(), finalLink);
                
                // read binary data if available
                if(!this->jsonifyLinkNNData && this->countFileLine(this->nnDataLoc) > 2)
                {
                    // need copy of speed prediction to get the number of layers used
                    SpeedPrediction sp;
                    
                    // map nn data to link hash
                    GenericMap<long int, std::vector<std::vector<Eigen::MatrixXd*>*>*> nnDataMap;
                    
                    // open binary containing link nn data
                    std::ifstream in(this->nnDataLoc.c_str(), std::ios::in | std::ios::binary);
                    
                    // matrix and matrix identifiers
                    Eigen::MatrixXd nnMat;
                    int matrixTypeNum;
                    int layerNum;
                    long int matLinkHash;
                    
                    // begin reading binary
                    while(!in.eof())
                    {
                        // read binary
                        this->readBinaryNNMat(in, nnMat, matrixTypeNum, layerNum, matLinkHash);
                        
                        // create new mat pointer and pointer to nn data container
                        Eigen::MatrixXd* newNNMat = new Eigen::MatrixXd(nnMat);
                        std::vector<std::vector<Eigen::MatrixXd*>*>* nnData;
                        
                        // grab nn data container if available
                        if(nnDataMap.hasEntry(matLinkHash))
                        {
                            nnData = nnDataMap.getEntry(matLinkHash);
                        }
                        
                        // create new container if not
                        else
                        {
                            nnData = new std::vector<std::vector<Eigen::MatrixXd*>*>(3);
                            
                            for(int i = 0; i < nnData->size(); i++)
                            {
                                nnData->at(i) = new std::vector<Eigen::MatrixXd*>(sp.getNumLayers());
                            }
                            
                            // add new nn data container to map of nn data to link hashes
                            nnDataMap.addEntry(matLinkHash, nnData);
                        }
                        
                        // update the map
                        if(nnData->at(matrixTypeNum)->at(layerNum) == NULL)
                        {
                            nnData->at(matrixTypeNum)->at(layerNum) = newNNMat;
                        }
                    }
                    in.close();
                    
                    // iterate through map of link nn data and update map of links
                    nnDataMap.initializeCounter();
                    GenericEntry<long int, std::vector<std::vector<Eigen::MatrixXd*>*>*>* nextNNData = nnDataMap.nextEntry();
                    while(nextNNData != NULL)
                    {
                        long int nextLinkHash = nextNNData->key;
                        std::vector<std::vector<Eigen::MatrixXd*>*>* nnData = nextNNData->value;
                        
                        if(links->hasEntry(nextLinkHash))
                        {                        
                            Link* storedLink = links->getEntry(nextLinkHash);
                            std::cout << storedLink->getNumber() << std::endl;
                            storedLink->setWeights(nnData->at(0), nnData->at(1), nnData->at(2), storedLink->getDirection());
                        }
                            
                        nextNNData = nnDataMap.nextEntry();
                    }
                    delete(nextNNData);
                }
            }
            
            // GOALS
            else if(!rpData.compare("GOALS"))
            {
                std::cout << "in get rp, goals" << std::endl;
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
                std::cout << "in get rp, link2state" << std::endl;
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
                std::cout << "in get rp, goal2link" << std::endl;
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
    
    return new RoutePrediction();
}

City* DataManagement::getCityData()
{
    std::cout << "getting city data" << std::endl;
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
				if(!child.compare("roads"))
                {
                    
                    GenericMap<long int, GenericMap<long int, Node*>*> roadsNodes;
                    
                    if(!this->jsonifyRoadNodes)
                    {
                        std::ifstream in(this->nodeDataLoc.c_str(), std::ios::in | std::ios::binary);
                        
                        Eigen::MatrixXd nodeMat;
                        long int roadID;
                        
                        while(!in.eof())
                        {
                            this->readBinaryNodeMat(in, nodeMat, roadID);
                            
                            GenericMap<long int, Node*>* roadNodes = new GenericMap<long int, Node*>();
                            for(int i = 0; i < nodeMat.rows(); i++)
                            {
                                double lat = nodeMat.coeffRef(i, 0);
                                double lon = nodeMat.coeffRef(i, 1);
                                float ele = nodeMat.coeffRef(i, 2);
                                long int id = nodeMat.coeffRef(i, 3);
                                
                                Node* newNode = new Node(lat, lon, ele, id);
                                
                                roadNodes->addEntry(i, newNode);
                            }
                            
                            roadsNodes.addEntry(roadID, roadNodes);
                        }
                        
                        in.close();
                    }
                    
                    std::cout << "in get city, roads" << std::endl;
					BOOST_FOREACH(ptree::value_type& z, v.second)
					{
                        GenericMap<int, double> nodeLats;
                        GenericMap<int, double> nodeLons;
                        GenericMap<int, int> nodeEles;
                        GenericMap<int, long int> nodeIDs;

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
							} else if(!roadFeature.compare("nodes") && this->jsonifyRoadNodes) {
								int latCount = 0; int lonCount = 0; int eleCount = 0; int idCount = 0;
								BOOST_FOREACH(ptree::value_type& b, a.second)
								{
									std::string nodeFeature = b.first.data();
									BOOST_FOREACH(ptree::value_type& c, b.second)
									{
										if(!nodeFeature.compare("latitude")) {
											nodeLats.addEntry(latCount, lexical_cast<double>(c.second.data()));
                                            latCount++;
										} else if(!nodeFeature.compare("longitude")) {
											nodeLons.addEntry(lonCount, lexical_cast<double>(c.second.data()));
                                            lonCount++;
										} else if(!nodeFeature.compare("elevation")) {
											nodeEles.addEntry(eleCount, lexical_cast<int>(c.second.data()));
                                            eleCount++;
										} else if(!nodeFeature.compare("nodeIDs")) {
											nodeIDs.addEntry(idCount, lexical_cast<long int>(c.second.data()));
                                            idCount++;
										}
									}
								}
							}
						}
                        
                        GenericMap<long int, Node*>* nodes;
                        
//                        // fit spline
//                        typedef Eigen::Spline<double, 2> spline2f;
//                        spline2f roadSpline;
                        
                        if(this->jsonifyRoadNodes)
                        {
                            assert(nodeLats.getSize() == nodeLons.getSize());
                            assert(nodeLons.getSize() == nodeEles.getSize());
                            assert(nodeEles.getSize() == nodeIDs.getSize());
                            
//                            // for splines
//                            Eigen::MatrixXd points(2, nodeLats.getSize());

                            nodes = new GenericMap<long int, Node*>();
                            for(int i = 0; i < nodeLats.getSize(); i++)
                            {
//                                points(0, i) = nodeLats.getEntry(i);
//                                points(1, i) = nodeLons.getEntry(i);
                                
                                nodes->addEntry(i, new Node(nodeLats.getEntry(i), nodeLons.getEntry(i), nodeEles.getEntry(i), nodeIDs.getEntry(i)));
                            }
                            
//                            roadSpline = Eigen::SplineFitting<spline2f>::Interpolate(points, 1);
                        }
                        else
                        {
                            assert(roadsNodes.hasEntry(roadID));
                            nodes = roadsNodes.getEntry(roadID);
                            
//                            // for splines
//                            Eigen::MatrixXd points(2, nodes->getSize());
//                            
//                            for(int i = 0; i < nodes->getSize(); i++)
//                            {
//                                points(0, i) = nodes->getEntry(i)->getLat();
//                                points(1, i) = nodes->getEntry(i)->getLon();
//                            }
//                            
//                            roadSpline = Eigen::SplineFitting<spline2f>::Interpolate(points, 1);
                        }
                
						roadIntersections->addEntry(roadID, new std::pair<int, int>(startNodeID, endNodeID));
                        
                        Road* road = new Road(roadType, roadID, nodes);
                        
                        road->assignSplineLength(splineLength);
//                        road->assignSpline(roadSpline);
                        road->setMinMaxLatLon();
                        road->setBoundsID(boundsID);
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
				if(!child.compare("intersections"))
                {
                    std::cout << "in get city, intersections" << std::endl;
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
                    std::cout << "in get city, bounds" << std::endl;
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
					Bounds* newBounds = new Bounds(maxLat, minLat, maxLon, minLon);
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
						} else if(!subTree.compare("longitude")) {
							double lon = lexical_cast<double>(b.second.data());
							lons->addEntry(lonCount++, lon);

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
    
template<class Matrix>
void DataManagement::writeBinaryNNMat(std::ofstream& out, const Matrix& matrix, int matrixTypeNum, int layerNum, long int linkHash)
{
    // matrix id
    out.write((char*) (&matrixTypeNum), sizeof(matrixTypeNum));
    out.write((char*) (&layerNum), sizeof(layerNum));
    out.write((char*) (&linkHash), sizeof(linkHash));
    
    // save matrix
    typename Matrix::Index rows=matrix.rows(), cols=matrix.cols();
    out.write((char*) (&rows), sizeof(typename Matrix::Index));
    out.write((char*) (&cols), sizeof(typename Matrix::Index));
    out.write((char*) matrix.data(), rows*cols*sizeof(typename Matrix::Scalar) );
}
    
template<class Matrix>
void DataManagement::readBinaryNNMat(std::ifstream& in, Matrix& matrix, int& matrixTypeNum, int& layerNum, long int& linkHash)
{
    // get matrix id
    in.read((char*) (&matrixTypeNum), sizeof(matrixTypeNum));
    in.read((char*) (&layerNum), sizeof(layerNum));
    in.read((char*) (&linkHash), sizeof(linkHash));
    
    // get matrix
    typename Matrix::Index rows=0, cols=0;
    in.read((char*) (&rows),sizeof(typename Matrix::Index));
    in.read((char*) (&cols),sizeof(typename Matrix::Index));
    matrix.resize(rows, cols);
    in.read( (char *) matrix.data() , rows*cols*sizeof(typename Matrix::Scalar) );
}

template<class Matrix>
void DataManagement::writeBinaryNodeMat(std::ofstream& out, const Matrix& matrix, long int roadID)
{
    // matrix id
    out.write((char*) (&roadID), sizeof(roadID));
    
    // save matrix
    typename Matrix::Index rows=matrix.rows(), cols=matrix.cols();
    out.write((char*) (&rows), sizeof(typename Matrix::Index));
    out.write((char*) (&cols), sizeof(typename Matrix::Index));
    out.write((char*) matrix.data(), rows*cols*sizeof(typename Matrix::Scalar) );
}

template<class Matrix>
void DataManagement::readBinaryNodeMat(std::ifstream& in, Matrix& matrix, long int& roadID)
{
    // get matrix id
    in.read((char*) (&roadID), sizeof(roadID));
    
    // get matrix
    typename Matrix::Index rows=0, cols=0;
    in.read((char*) (&rows),sizeof(typename Matrix::Index));
    in.read((char*) (&cols),sizeof(typename Matrix::Index));
    matrix.resize(rows, cols);
    in.read( (char *) matrix.data() , rows*cols*sizeof(typename Matrix::Scalar) );
}
    
}





