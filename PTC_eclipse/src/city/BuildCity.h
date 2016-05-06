/*
 * BuildCity.h
 *
 *  Created on: Apr 4, 2016
 *      Author: vagrant
 */

#ifndef CITY_BUILDCITY_H_
#define CITY_BUILDCITY_H_

#define EIGEN_NO_DEBUG

#include "City.h"
#include "../data_management/DataManagement.h"
#include "../data_management/DataCollection.h"
#include "../map/GenericMap.h"

#include "limits.h"
#include <eigen3/Eigen/Core>
#include <eigen3/unsupported/Eigen/Splines>

namespace PredictivePowertrain {

class BuildCity {
private:
	City city;
	GenericMap<int, Intersection*> intersections;
	GenericMap<int, Road*> roads;
	Eigen::MatrixXd adjMatFromSplines;

	double splineStep = 0.025;
	double adjMatPrecFromSplines = 0.00009;
	double gpsTolerance = 0.0001;
	double idScalar = 10000.0;
	bool newBounds = false;

	std::pair<GenericMap<int, Intersection*>*, GenericMap<long int, Road*>*>* parseAdjMat();

public:
	BuildCity();
	void updateGridData(); // post-processing on shutdown
	void printAdjMats();
	bool hasNewBounds();
};

}

#endif /* CITY_BUILDCITY_H_ */
