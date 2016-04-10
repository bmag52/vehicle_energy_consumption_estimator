/*
 * Node.h
 *
 *  Created on: Apr 8, 2016
 *      Author: vagrant
 */

#ifndef OSM_NODE_H_
#define OSM_NODE_H_

namespace PredictivePowertrain {

class Node {
private:
	double lat;
	double lon;
	double ele;
	int id;

public:
	Node();
	Node(double lat, double lon, int id);
	double getLat();
	double getLon();
	int getID();
	double getEle();
};

}

#endif /* OSM_NODE_H_ */
