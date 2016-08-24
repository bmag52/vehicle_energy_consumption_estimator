/*
 * Bounds.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#ifndef DATA_MANAGEMENT_BOUNDS_H_
#define DATA_MANAGEMENT_BOUNDS_H_

#include <utility>

namespace PredictivePowertrain {
    
class Bounds {
private:
	double maxLat;
	double maxLon;
	double minLat;
	double minLon;
	int id;

public:
	Bounds();
	Bounds(double maxLat, double maxLon, double minLat, double minLon);
	double getMaxLat();
	double getMaxLon();
	double getMinLat();
	double getMinLon();
    std::pair<double, double>* getMidLatLon();
	void assignID(int id);
	int getID();
};

} /* namespace PredictivePowertrain */

#endif /* DATA_MANAGEMENT_BOUNDS_H_ */
