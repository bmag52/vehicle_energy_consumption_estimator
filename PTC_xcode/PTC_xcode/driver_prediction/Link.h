/*
 * Link.h
 *
 *  Created on: Jan 5, 2016
 *      Author: Amanda
 */

#ifndef LINK_H_
#define LINK_H_

#include "../city/Road.h"
#include "../city/Intersection.h"

#include <eigen3/Eigen/Dense>
#include <list>

namespace PredictivePowertrain {

class Intersection; // forward declaration

class Link {
private:
    long int link_number;
    int link_direction;
    
    // input weights on A-end of link
    Eigen::MatrixXd * WtsA;		// matrix of weights
    Eigen::MatrixXd * yHidA;	// hidden layer outputs
    Eigen::MatrixXd * yInHidA;	// hidden layer inputs
    
    // input weights on B-end of link
    Eigen::MatrixXd * WtsB;		// matrix of weights
    Eigen::MatrixXd * yHidB;	// hidden layer outputs
    Eigen::MatrixXd * yInHidB;	// hidden layer inputs
    
    int numNNLayers;
    void initialize();
    
public:
	Link();
	Link(int, long int);
	virtual ~Link();
	Link* copy(int, long int);
	long int getHash();
	bool isEqual(Link* other);
    bool isFinalLink();
	long int getNumber();
	int getDirection();
	static Link* newLinkFromHash(long int);
	static Link* finalLink();
	Link* linkFromRoad(Road* road, Intersection* intersection);
    void setWeights(Eigen::MatrixXd* wts, Eigen::MatrixXd* yHid, Eigen::MatrixXd* yInHid, int direction);
    std::list<Eigen::MatrixXd*>* getWeights(int direction);
    void setNumNNLayers(int num);
    int getNumNNLayers();
};

} /* namespace PredictivePowertrain */

#endif /* LINK_H_ */
