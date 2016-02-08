#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "linefinder.h"

#define PI 3.1415926

using namespace cv;
using namespace std;

static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// the function draws all the squares in the image
static void drawContours( Mat& image, const vector<vector<Point> >& contours )
{
    for( size_t i = 0; i < contours.size(); i++ )
    {
        const Point* p = &contours[i][0];
        int n = (int)contours[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3);
    }
    
    imshow("contours", image);
}

int main() {
    
    // Set-Up
    int showSteps = 1;
    string arg = "/Users/Brian/Desktop/school/all_classes/EE576/car_CV/trafficSignalDetection/finalProjectVidEE576.mp4";

    
    // Capture Input
    VideoCapture capture(arg);
    
    // capture.set(CV_CAP_PROP_POS_MSEC, 100000); //start the video at 100 seconds in
    
    // Process Frame
    Mat image;
    double frameItr = 0;
    
    
    image = imread(arg);
    while (1)
    {
        // capture on intervals to make vid smoother
        capture >> image;
        frameItr += 300;
        capture.set(CV_CAP_PROP_POS_MSEC, frameItr);
        
        if (image.empty())
            break;
        
        // convert to greyscale
        Mat greyMat;
        cvtColor(image, greyMat, CV_BGR2GRAY);
        if(showSteps) {
            namedWindow("greyscale");
            imshow("greyscale",greyMat);
        }
        
        // Convert to binary image using Canny
        Mat canned;
        Canny(greyMat, canned, 0, 50, 5);
        if(showSteps) {
            namedWindow("canny");
            imshow("canny", canned);
        }
        
        // search for contours
        vector<vector<Point> > contours;
        findContours(canned.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    
        vector<Point> contourStore;
        vector<vector<Point> > squares;
        vector<vector<Point> > hexagons;
        
        for (int i = 0; i < contours.size(); i++) {
            // Approximate contour with accuracy proportional
            // to the contour perimeter
            approxPolyDP(
                        Mat(contours[i]),
                        contourStore,
                        arcLength(cv::Mat(contours[i]), true) * 0.02,
                        true
                        );
            
            // Skip small or non-convex objects
            if (fabs(contourArea(contours[i])) < 1025 || !cv::isContourConvex(contourStore))
                continue;
            
            // calculate all angles for contour
            int vtc = contourStore.size();
            std::vector<double> cos;
            for (int j = 2; j < vtc+1; j++)
                cos.push_back(angle(contourStore[j%vtc], contourStore[j-2], contourStore[j-1]));
            
            // Sort ascending the corner degree values and find max and min values
            std::sort(cos.begin(), cos.end());
            double minCosine = cos.front();
            double maxCosine = cos.back();
            
            int xCoord = contours[i][0].x;
            int yCoord = contours[i][0].y;
            // look for red
            Vec3b color = image.at<Vec3b>(Point(xCoord, yCoord));
            
            // look for rectangular speed limit signs
            if(vtc == 4 && minCosine >= -0.1 && maxCosine < 0.3
               && (color[2] < 50 || color[2] > 200)) {
                squares.push_back(contourStore);
                cout << " Speed Limit 20 mph \n";
            // look for octagonal stop signs as red circles
            } else {
                if((color[2] > 100 || color[2] < 5) && (vtc >= 5)) {
                    hexagons.push_back(contourStore);
                    cout << "stop \n";
                }
            }
        }
        
        drawContours(image, squares);
        drawContours(image, hexagons);
        
        // show window;
        namedWindow("stuff", WINDOW_AUTOSIZE);
        imshow("vid", image);
        
        waitKey(1);
        //cout << " frameItr " << frameItr << "\n";
    }
}

