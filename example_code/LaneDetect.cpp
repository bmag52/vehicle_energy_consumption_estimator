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

int main() {
    
    // Set-Up
    int linePointCount = 250;
    string arg = "/Users/Brian/Desktop/school/all_classes/EE576/car_CV/video/pikes peak.mp4";
    
    // Set up windows
    bool showROI = 0;
    bool displayCannyOutput = 1;
    bool displayHoughOutput = 1;
    bool displayHoughOutputP = 0;
    
    // vidFrame Input
    string window_name = "Test Video";
    namedWindow(window_name, CV_WINDOW_KEEPRATIO);
    VideoCapture vidFrame(arg);
    
    vidFrame.set(CV_CAP_PROP_POS_MSEC, 100000);
    
    // Process Frame
    Mat image;
    double frameItr = 0;
    image = imread(arg);
    int crestCount = 0, frameSkip = 0;
    while (1)
    {
        // vidFrame on intervals to make vid smoother
        vidFrame >> image;
        frameItr += 100;
        vidFrame.set(CV_CAP_PROP_POS_MSEC, frameItr);
        
        if (image.empty())
            break;
        
        Mat gray;
        cvtColor(image,gray,CV_RGB2GRAY);
        vector<string> codes;
        Mat corners;
        findDataMatrix(gray, codes, corners);
        drawDataMatrixCodes(image, codes, corners);
        
        // rangeOfInterest
        // optimized? -=> yes
        int top = 160;
        int left = 160; //
        int width = 350;
        int height = 120;
        
        Rect rangeOfInterest(left,top,width,height);
        Mat imgRegionOfInterest = image(rangeOfInterest);
        Scalar val = Scalar(0, 0, 0);
        copyMakeBorder(imgRegionOfInterest, imgRegionOfInterest, 2, 2, 2, 2, BORDER_CONSTANT, val);
        
        // Display the region of interest
        if(showROI) {
            namedWindow("Original Image");
            imshow("Original Image",imgRegionOfInterest);
        }
        
        // Canny
        Mat contours;
        Canny(imgRegionOfInterest,contours,100,250);
        Mat contoursInv;
        threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);
        
        // Display Canny image
        if(displayCannyOutput) {
            namedWindow("Canny");
            imshow("Canny1",contoursInv);
        }
        
        // hough line transform
        std::vector<Vec2f> lines;
        if (linePointCount < 1 or lines.size() > 2) {
            linePointCount = 200;
        }
        else{ linePointCount += 25;}
        while(lines.size() < 5 && linePointCount > 0){
            HoughLines(contours,lines,1,PI/180, linePointCount);
            linePointCount -= 5;
        }

        Mat houghLineResult(imgRegionOfInterest.size(),CV_8U,Scalar(255));
        imgRegionOfInterest.copyTo(houghLineResult);
        
        // Draw the hough lines
        std::vector<Vec2f>::const_iterator it= lines.begin();
        Mat hough(imgRegionOfInterest.size(),CV_8U,Scalar(0));
        while (it!=lines.end()) {
            
            float mag= (*it)[0];
            float lineAngle= (*it)[1];
            
            if ( (lineAngle > 0.1 && lineAngle < 1.48) || (lineAngle < 3.14 && lineAngle > 1.67) ) {
                Point pt1(mag/cos(lineAngle),0);
                Point pt2((mag-houghLineResult.rows*sin(lineAngle))/cos(lineAngle),houghLineResult.rows);
                line( houghLineResult, pt1, pt2, Scalar(255,255,255), 1);
                line( hough, pt1, pt2, Scalar(255,255,255), 1);
            }
            
            ++it;
        }
        
        // Display the detected hough lines
        if(displayHoughOutput){
            namedWindow("Hough lines");
            imshow("Hough lines",houghLineResult);
        }
        
        // Create lineManager instance
        lineManager ld;
        
        // Set end-point Hough parameters
        ld.setMaxLineLengthAndGap(30,60);
        ld.setMinLinePointCount(15);
        
        // find lines
        std::vector<Vec4i> li= ld.findLines(contours);
        Mat houghP(imgRegionOfInterest.size(),CV_8U,Scalar(0));
        ld.setROIShift(0,0);
        ld.drawDetectedLines(houghP);
        
        if(displayHoughOutputP){
            namedWindow("Detected Lines with HoughP");
            imshow("Detected Lines with HoughP", houghP);
        }
        
        // superimpose images
        bitwise_and(houghP,hough,houghP);
        Mat houghPinv(imgRegionOfInterest.size(),CV_8U,Scalar(0));
        Mat dst(imgRegionOfInterest.size(),CV_8U,Scalar(0));
        threshold(houghP,houghPinv,150,255,THRESH_BINARY_INV);
        
        if(displayHoughOutputP){
            namedWindow("found lines - end points");
            imshow("found lines - end points", houghPinv);
        }
        
        
        Canny(houghPinv,contours,100,350);
        li = ld.findLines(contours);
        // Display Canny image
        if(displayCannyOutput){
            namedWindow("Contours");
            imshow("Contours2",contours);
        }
        
        // define tighter parameters for hough end point transform
        ld.setMaxLineLengthAndGap(5,2);
        ld.setMinLinePointCount(1);
        ld.setROIShift(top, left);
        
        // draw point on image where line intersection occurs
        int yShift = 25;
        int allowableFrameSkip = 5;
        ld.drawDetectedLines(image);
        cv::Point iPnt = ld.drawIntersectionPunto(image, 2);
        
        // track hill crest
        int gap = 20;
        cv::Point lptl(0, image.rows / 2 + yShift);
        cv::Point lptr(gap, image.rows / 2 + yShift);
        line(image, lptl, lptr, Scalar(255, 255, 255), 1);// left mid line
        
        cv::Point rptl(image.cols - gap, image.rows / 2 + yShift);
        cv::Point rptr(image.cols, image.rows / 2 + yShift);
        line(image, rptl, rptr, Scalar(255, 255, 255), 1);// right mid line
        
        cv::Point ulpt(0, image.rows / 2 - 50 + yShift);
        cv::Point urpt(image.cols, image.rows / 2 - 50 + yShift);
        line(image, ulpt, urpt, Scalar(255, 255, 255), 1);// upper line
        
        bool hillCrestFound = (iPnt.y < (image.rows / 2 + yShift)) && (iPnt.y > (image.rows / 2 - 50 + yShift));
        if(hillCrestFound) {
            crestCount++;
            frameSkip = 0;
        } else if(crestCount != 0 && frameSkip < allowableFrameSkip)
            frameSkip++;
        else {
            crestCount = 0;
            frameSkip = 0;
        }
    
        cv::Point txtPt(image.cols / 2 - 31, image.rows / 2 + 140);
        if(crestCount > 3)
            putText(image, "tracking", txtPt, FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 2, 8);
        
        std::stringstream stream;
        stream << "Lines Segments: " << lines.size();
        
        putText(image, stream.str(), Point(10,image.rows-10), 1, 0.8, Scalar(0,255,0),0);
        imshow(window_name, image);
        
        char key = (char) waitKey(10);
        lines.clear();
    }
}