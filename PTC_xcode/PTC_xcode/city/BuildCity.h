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

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace PredictivePowertrain {
    
    class BuildCity {
    private:
        // functional
        City city;
        GenericMap<long int, Road*>* rawRoads;
        Eigen::MatrixXd adjMatFromSplines;
        double maxLat;
        double maxLon;
        double minLat;
        double minLon;
        double latCenter;
        double lonCenter;
        double latDelta;
        double lonDelta;
        
        int boundsID = 0;
        double idScalar = 10000.0;
        bool newBounds = false;
        
        // for spline shit using osm oxl
        double splineStep = 0.025;
        double adjMatPrecFromSplines = 0.00001;
        double gpsTolerance = 0.0001;
        
        
        std::pair<GenericMap<int, Intersection*>*, GenericMap<long int, Road*>*>* parseAdjMat();
        double scaleID(long int id);
        long int unScaleID(double id);
        void connectifyAjdMat();
        bool isAdj(GenericEntry<int, std::pair<int, int>*>* idx1, GenericEntry<int, std::pair<int, int>*>* idx2);
        std::pair<DataCollection*, Bounds*>* setupDataCollection();
        cv::Mat pullAndFormatMapPNG(DataCollection* dc);
        
    public:
        BuildCity();
        void updateGridDataXML();
        void updateGridDataPNG();
        void printAdjMats();
        bool hasNewBounds();
    };
    
}

#endif /* CITY_BUILDCITY_H_ */