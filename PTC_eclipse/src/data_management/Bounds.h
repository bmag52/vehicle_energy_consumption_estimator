/*
 * Bounds.h
 *
 *  Created on: Apr 18, 2016
 *      Author: vagrant
 */

#ifndef DATA_MANAGEMENT_BOUNDS_H_
#define DATA_MANAGEMENT_BOUNDS_H_

namespace PredictivePowertrain {

class Bounds {
private:
	double maxLat;
	double maxLon;
	double minLat;
	double minLon;

public:
	Bounds();
	Bounds(double maxLat, double maxLon, double minLat, double minLon);
	double getMaxLat();
	double getMaxLon();
	double getMinLat();
	double getMinLon();
};

} /* namespace PredictivePowertrain */

#endif /* DATA_MANAGEMENT_BOUNDS_H_ */
