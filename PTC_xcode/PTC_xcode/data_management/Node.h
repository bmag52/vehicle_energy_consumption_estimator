/*
 * Node.h
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#ifndef DATACOLLECTION_NODE_H_
#define DATACOLLECTION_NODE_H_

namespace PredictivePowertrain {

class Node {
private:
	double lat;
	double lon;
	float ele;
	int long id;

public:
	Node();
	Node(double lat, double lon, float ele, long int id);
	double getLat();
	double getLon();
	long int getID();
	float getEle();
    void setEle(float newEle);
};

}

#endif /* DATACOLLECTION_NODE_H_ */
