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
#include <math.h>

namespace PredictivePowertrain {

class BuildCity {
private:
	City city;
	GenericMap<long int, Road*>* rawRoads;
	Eigen::MatrixXd adjMatFromSplines;

	double splineStep = 0.025;
	double adjMatPrecFromSplines = 0.00009;
	double gpsTolerance = 0.0001;
	double idScalar = 10000.0;
	bool newBounds = false;

	std::pair<GenericMap<int, Intersection*>*, GenericMap<long int, Road*>*>* parseAdjMat();
	double scaleID(long int id);
	long int unScaleID(double id);
	void connectifyAjdMat();
	bool isAdj(GenericEntry<int, std::pair<int, int>*>* idx1, GenericEntry<int, std::pair<int, int>*>* idx2);
public:
	BuildCity();
	void updateGridData(); // post-processing on shutdown
	void printAdjMats();
	bool hasNewBounds();
};

}

#endif /* CITY_BUILDCITY_H_ */
