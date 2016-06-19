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
	float lat;
	float lon;
	int ele;
	int long id;

public:
	Node();
	Node(float lat, float lon, int ele, long int id);
	float getLat();
	float getLon();
	long int getID();
	int getEle();
};

}

#endif /* DATACOLLECTION_NODE_H_ */
