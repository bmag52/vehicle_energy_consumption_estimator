#if !defined LINEF
#define LINEF

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define PI 3.1415926

class lineManager {
    
private:
    
    // original image
    cv::Mat image;
    
    // line vector
    std::vector<cv::Vec4i> lines;
    
    // line parameters
    double mag;
    double lineAngle;
    
    // minimum number point a line must have before considered a line
    int minLinePointCount;
    
    // minimum line length
    double minLineLength;
    
    // maximum space allowed between line points
    double maxLineGap;
    
    // shifts for roi
    int shiftV;
    int shiftH;
    
public:
    
    // how to access lineManager class. Default values specified below.
    lineManager() : mag(1), lineAngle(PI/180), minLinePointCount(10), minLineLength(0.), maxLineGap(0.) {}
    
    // Set precision of line manager
    void setLMPrecision(double magIn, double lineAngleIn) {
        
        mag= magIn;
        lineAngle= lineAngleIn;
    }
    
    // Set the minimum number of votes
    void setMinLinePointCount(int minPuntoCount) {
        
        minLinePointCount= minPuntoCount;
    }
    
    // Set line length and gap
    void setMaxLineLengthAndGap(double l, double g) {
        
        minLineLength= l;
        maxLineGap= g;
    }
    
    // set image shift
    void setROIShift(int Vshift, int Hshift) {
        
        shiftV = Vshift;
        shiftH = Hshift;
    }
    
    // Hough transform with end points
    std::vector<cv::Vec4i> findLines(cv::Mat& binary) {
        
        lines.clear();
        cv::HoughLinesP(binary,lines,mag,lineAngle,minLinePointCount, minLineLength, maxLineGap);
        
        return lines;
    }
    
    // Draw the detected lines on an image
    void drawDetectedLines(cv::Mat &image, cv::Scalar color=cv::Scalar(204,0,102)) {
        
        // Draw the lines
        std::vector<cv::Vec4i>::const_iterator it2= lines.begin();
        
        while (it2!=lines.end()) {
            
            cv::Point pt1((*it2)[0]+shiftH,(*it2)[1]+shiftV);
            cv::Point pt2((*it2)[2]+shiftH,(*it2)[3]+shiftV);
            
            //thickness = 1.5
            cv::line( image, pt1, pt2, color, 1.5 );
            
            std::cout << " HoughP line: ("<< pt1 <<"," << pt2 << ")\n";
            ++it2;
        }
    }
    
    cv::Point drawIntersectionPunto(cv::Mat &image, int tolerance) {
        
        std::vector<cv::Vec4i>::const_iterator it2= lines.begin();

        int cols = image.cols;
        int rows = image.rows;
        int intXCount[cols];
        int intYCount[rows];
        
        std::fill_n(intXCount, cols, 0);
        std::fill_n(intYCount, rows, 0);
        
        while(it2!=lines.end()) {
            
            cv::Point pt1((*it2)[0]+shiftH, (*it2)[1]+shiftV);
            cv::Point pt2((*it2)[2]+shiftH, (*it2)[3]+shiftV);
            
            std::vector<cv::Vec4i>::const_iterator it3= lines.begin();
            
            while(it3!=lines.end()) {
                
                cv::Point pt3((*it3)[0]+shiftH, (*it3)[1]+shiftV);
                cv::Point pt4((*it3)[2]+shiftH, (*it3)[3]+shiftV);
                
                cv::Point intPnt;
                bool intersect = getIntersectionPoint(pt1, pt2, pt3, pt4, intPnt);
                if(intersect) {
                    double theta1 = (*it2)[1];
                    double theta2 = (*it3)[1];
                    bool convergent = (theta1 > 1.48 && theta2 < 1.48) || (theta1 < 1.48 && theta2 > 1.48);
                    if(convergent && intPnt.x < cols && intPnt.y < rows) {
                        intXCount[intPnt.x] += 1;
                        intYCount[intPnt.y] += 1;
                    }
                    
//                    // red dot on midpoint!
//                    cv::Point mid = 0.5*(pt1+pt2); // midpoint
//                    drawPoint(image, mid);  // draw midpoint
                    
                    std::cout << " found point: ("<< intPnt <<") \n";
                }
                ++it3;
            }
            ++it2;
        }
        
        cv::Point intersectPt;
        int x = getCoord(intXCount, cols, tolerance);
        int y = getCoord(intYCount, rows, tolerance);
        if(x != -1) {
            intersectPt.x = x;
            intersectPt.y = y;
            drawPoint(image, intersectPt);
        }
        return intersectPt;
    }
    
    int getCoord(int* dimCount, int dim, int tol) {
        int i, coordCount = 0, max = 0, coord = -1, itr = 0;
        for(i = 0; i < dim; i++) {
            if(itr == tol) {
                if(coordCount > max) {
                    max = coordCount;
                    coord = i + tol / 2;
                }
                coordCount = 0;
                itr = 0;
            } else if(dimCount[i] != 0) {
                int c = dimCount[i];
                coordCount += c;
            }
            itr++;
        }
        return coord;
    }
    
    // Draw the detected intersection point on an image
    void drawPoint(cv::Mat &image, cv::Point center) {
        
        cv::Scalar color=cv::Scalar(0,0,255); // line color
        int rad = 4;
        cv::circle( image, center, rad, color, -1, 8 );
        
    }
    
    // get intersection points
    bool getIntersectionPoint(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2, cv::Point & intPnt){
        cv::Point p = a1;
        cv::Point q = b1;
        cv::Point r(a2-a1);
        cv::Point s(b2-b1);
        
        if(cross(r,s) == 0) {
            return false;}
        
        double t = cross(q-p,s)/cross(r,s);
        
        intPnt = p + t*r;
        std::cout << " Intersection: True\n";
        return true;
    }
    
    double cross(cv::Point v1,cv::Point v2){
        return v1.x*v2.y - v1.y*v2.x;
    }
    
};


#endif