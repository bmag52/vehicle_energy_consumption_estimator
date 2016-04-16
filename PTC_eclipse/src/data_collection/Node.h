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
	int ele;
	int long id;

public:
	Node();
	Node(double lat, double lon, int ele, int id);
	double getLat();
	double getLon();
	long int getID();
	int getEle();
};

}

#endif /* DATACOLLECTION_NODE_H_ */
