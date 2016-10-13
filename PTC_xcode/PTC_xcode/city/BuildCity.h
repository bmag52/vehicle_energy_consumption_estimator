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
    GenericMap<long int, Intersection*>* newInts;
    Eigen::MatrixXd adjMatFromSplines;
    double maxLat;
    double maxLon;
    double minLat;
    double minLon;
    double latCenter;
    double lonCenter;
    double latDelta;
    double lonDelta;
    
    struct zoomParams {
        int kernelSideDim;
        int angleThreshold;
        int pntThreshold;
        int imageProcessingResolution;
        int coordBinResolution;
        int maxLines;
        double maxClusterPntDistance;
    };
    
    int boundsID = 0;
    double idScalar = 10000.0;
    bool newBounds = false;
    
    // for spline shit using osm 
    double splineStep = 0.025; // s-value
    double evalIntervalLength = 5.0; // meters
    double adjMatPrecFromSplines = 0.00001;
    double gpsTolerance = 0.0001;
            
    std::pair<GenericMap<int, Intersection*>*, GenericMap<long int, Road*>*>* parseAdjMat();
    double scaleID(long int id);
    long int unScaleID(double id);
    void connectifyAjdMat();
    bool isAdj(GenericEntry<int, std::pair<int, int>*>* idx1, GenericEntry<int, std::pair<int, int>*>* idx2);
    std::pair<DataCollection*, Bounds*>* setupDataCollection();
    std::pair<int, cv::Mat>* pullAndFormatMapPNG(DataCollection* dc);
    template<typename K> bool getIntersectionPoint(cv::Point_<K> a1, cv::Point_<K> a2, cv::Point_<K> b1, cv::Point_<K> b2, cv::Point_<K> & intPnt);
    std::pair<cv::Point, cv::Point>* polarToCartisian(float mag, float angle, int rows);
    template<typename K> double cross(cv::Point_<K> v1, cv::Point_<K> v2);
    int getCoord(int* dimCount, int dim, int tol);
    GenericMap<int, cv::Point*>* getIntersectionPointsFromMapPNG(cv::Mat map, int zoom);
    GenericMap<int, std::pair<cv::Point*, cv::Point*>*>* perimeterScanKernelForRoads(cv::Mat kernel);
    void drawPoint(cv::Mat &image, cv::Point point);
    void checkNextPixel(int x, int y, GenericMap<int, cv::Point*>& points, cv::Mat& kernel, int& lastPixel);
    void declusterIntersectionPoints(cv::Point* rawIntPnt, GenericMap<int, cv::Point*>& rawIntPnts, GenericMap<int, cv::Point*>& intPnts, int kernelSideDim, double maxDistance);
    zoomParams getZoomParams(int zoom);
    int hashCoords(int x, int y);
    std::pair<GenericMap<int, Road*>*, GenericMap<int, Intersection*>*>* makeRoadsAndIntersections(GenericMap<int, cv::Point*>* rawInts, cv::Mat map);
    std::pair<bool, GenericMap<int, cv::Point*>*>* traceRoad(cv::Point* nextClosestPnt, cv::Point* pnt, cv::Mat map, GenericMap<int, cv::Point*>& searchedIntPnts);
    
public:
    BuildCity();
    virtual ~BuildCity();
    void updateGridDataXMLSpline();
    void updateGridDataXMLAdj();
    void updateGridDataPNG();
    void printAdjMats();
    void printIntersections();
    bool hasNewBounds();
};
    
}

#endif /* CITY_BUILDCITY_H_ */