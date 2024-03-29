/*
 * GPS.cpp
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#include "GPS.h"

namespace PredictivePowertrain {

GPS::GPS() {
	this->tripCount = 0;
    this->fd = -1;
    this->deltaXYTolerance = 10.0;
}
    
GPS::GPS(double refLat, double refLon)
{
    this->refLat = refLat;
    this->refLon = refLon;
    this->tripCount = 0;
    this->fd = -1;
    this->deltaXYTolerance = 10.0;
}
    
GPS::~GPS()
{
    close(this->fd);
}

std::pair<double, double>* GPS::updateTripLog()
{
	std::pair<double, double> latLonRef = this->readGPS();
    std::cout << "GPS: " << latLonRef.first << "," << latLonRef.second << std::endl;
    
    std::pair<double, double>* latLonPtr = new std::pair<double, double>(latLonRef.first, latLonRef.second);
	this->tripLog.addEntry(this->tripCount, latLonPtr);
    this->tripCount++;
    
    return latLonPtr;
}

GenericMap<long int, std::pair<double, double>*>* GPS::getTripLog(bool interpolated)
{
    if(interpolated)
    {
        GenericMap<long int, std::pair<double, double>*> beforeInterpTripLog;
        GenericMap<long int, std::pair<double, double>*> afterInterpTripLog;
        this->interpolateTripLog(&this->tripLog, &beforeInterpTripLog);
        
        for(int i = 0; i < 1; i++)
        {
            this->interpolateTripLog(&beforeInterpTripLog, &afterInterpTripLog);
            beforeInterpTripLog = afterInterpTripLog;
        }
        
        return afterInterpTripLog.copy();
    }
    
    return this->tripLog.copy();
}

void GPS::interpolateTripLog(GenericMap<long int, std::pair<double, double>*>* before, GenericMap<long int, std::pair<double, double>*>* after)
{
    long int latLonCount = 0;
    
    before->initializeCounter();
    GenericEntry<long int, std::pair<double, double>*>* prevMeas = before->nextEntry();
    GenericEntry<long int, std::pair<double, double>*>* currMeas = before->nextEntry();
    while(currMeas != NULL)
    {
        double midLat = (prevMeas->value->first + currMeas->value->first) / 2;
        double midLon = (prevMeas->value->second + currMeas->value->second) / 2;
        
        after->addEntry(latLonCount++, new std::pair<double, double>(prevMeas->value->first, prevMeas->value->second));
        after->addEntry(latLonCount++, new std::pair<double, double>(midLat, midLon));
        
        prevMeas = currMeas;
        currMeas = before->nextEntry();
    }
    after->addEntry(latLonCount++, new std::pair<double, double>(prevMeas->value->first, prevMeas->value->second));
    
    delete(prevMeas);
    delete(currMeas);
    
}
    
// http://www.movable-type.co.uk/scripts/latlong.html
float GPS::deltaLatLonToXY(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = toRadians(lat2-lat1);
    double dLon = toRadians(lon2-lon1);
    
    double a = std::sin(dLat/2) * std::sin(dLat/2) +
    std::cos(toRadians(lat1)) * std::cos(toRadians(lat2)) *
    std::sin(dLon/2) * std::sin(dLon/2);
    
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    float dist = (float) (EARTH_RADIUS * c);
    
    return dist;
}
    
std::pair<double, double>* GPS::convertLatLonToXY(double lat, double lon)
{
    float dist = deltaLatLonToXY(lat, lon, this->refLat, this->refLon);
    double angle = std::atan2(lat - this->refLat, lon - this->refLon);
    return new std::pair<double, double>(dist * std::sin(angle), dist * std::cos(angle));
}
    
std::pair<double, double>* GPS::convertXYToLatLon(double x, double y)
{
    double latDelta = y/EARTH_RADIUS;
    double lonDelta = x/(EARTH_RADIUS*std::cos(this->refLat));
    
    return new std::pair<double, double>(this->refLat+toDegrees(latDelta), this->refLon+toDegrees(lonDelta));
}
    
double GPS::toDegrees(double radians)
{
    return radians / (2 * M_PI) * 360.0;
}
    
double GPS::toRadians(double degrees)
{
    return degrees / 180.0 * M_PI;
}
    
void GPS::initializeGPSReader()
{
    std::cout << "initializing gps" << std::endl;
    this->fd = open("/dev/tty.usbmodemFA131", O_RDONLY | O_NONBLOCK);
    if(this->fd < 0)
    {
        std::cout << "Unable to open /dev/tty." << std::endl;
    }
    
    struct termios theTermios;
    
    memset(&theTermios, 0, sizeof(struct termios));
    cfmakeraw(&theTermios);
    cfsetspeed(&theTermios, 115200);
    
    theTermios.c_cflag = CREAD | CLOCAL;     // turn on READ
    theTermios.c_cflag |= CS8;
    theTermios.c_cc[VMIN] = 0;
    theTermios.c_cc[VTIME] = 10;     // 1 sec timeout
    ioctl(this->fd, TIOCSETA, &theTermios);
}
    
std::pair<double, double> GPS::readGPS()
{
    if(this->fd < 0)
    {
        this->initializeGPSReader();
    }
    
    // define vars
    char buf[255];
    size_t res;
    
    while(true) // wait for read
    {
        res = read(this->fd,buf,255);
        
        if(res > 0)
        {
            buf[res]=0;
            
            std::string nmeaMsg(buf);
            std::stringstream ss(nmeaMsg);
            
            std::string token;
            while(std::getline(ss, token, ','))
            {
                if(!token.compare("$GNGLL"))
                {
                    std::string latString;
                    std::string lonString;
                    std::getline(ss, latString, ',');
                    std::getline(ss, token, ',');
                    std::getline(ss, lonString, ',');
                    
                    // break if bad read
                    if(latString.length() == 0 || lonString.length() == 0)
                    {
                        break;
                    }
                    // get raw deg / min values
                    double latRaw = std::stod(latString);
                    double lonRaw = std::stod(lonString);
                    
                    // get minutes w/o minute fractions
                    double latMin = (int)latRaw % 100;
                    double lonMin = (int)lonRaw % 100;
                    
                    // get degrees x100
                    double latDeg = ((int)latRaw - latMin);
                    double lonDeg = ((int)lonRaw - lonMin);
                    
                    // add minute fractions to minutes
                    latMin += latRaw - latDeg - latMin;
                    lonMin += lonRaw - lonDeg - lonMin;
                    
                    // divide lat / lon
                    latDeg /= 100;
                    lonDeg /= 100;
                    
                    // add minutes converted to degrees to lat / lon degrees
                    latDeg += latMin / 60;
                    lonDeg += lonMin / 60;
                    
                    //printf("%.6f,%.6f\n", lat, lon);
                    std::pair<double, double> latLon(latDeg, -lonDeg);
                    return latLon;
                    break;
                }
                else
                {
                    break;
                }
            }
        }
    }
}
    
bool GPS::isOnRoad(Road* road)
{
    if(road == NULL)
    {
        return false;
    }
    
    std::pair<double, double> latLon = this->readGPS();
    
    road->getNodes()->initializeCounter();
    GenericEntry<long int, Node*>* nextNode = road->getNodes()->nextEntry();
    while(nextNode != NULL)
    {
        if(this->deltaLatLonToXY(latLon.first, latLon.second, nextNode->value->getLat(), nextNode->value->getLon()) < this->deltaXYTolerance)
        {
            return true;
        }
        nextNode = road->getNodes()->nextEntry();
    }
    delete(nextNode);
    
    return false;

}

bool GPS::isAtIntersection(Intersection* intersection)
{
    std::pair<double, double> latLon = this->readGPS();
    
    if(this->deltaLatLonToXY(latLon.first, latLon.second, intersection->getLat(), intersection->getLon()) < this->deltaXYTolerance)
    {
        return true;
    }
    return false;
}
    
Road* GPS::getCurrentRoad2(City* city, double lat, double lon)
{
    GenericMap<long int, Road*>* roads = city->getRoads();
    Road* closestRoad = NULL;
    float closestDist = MAXFLOAT;
    
    roads->initializeCounter();
    GenericEntry<long int, Road*>* nextRoad = roads->nextEntry();
    while(nextRoad != NULL)
    {
        GenericMap<long int, Node*>* roadNodes = nextRoad->value->getNodes();
        
        roadNodes->initializeCounter();
        GenericEntry<long int, Node*>* nextNode = roadNodes->nextEntry();
        while(nextNode != NULL)
        {
            float currDist = this->deltaLatLonToXY(lat, lon, nextNode->value->getLat(), nextNode->value->getLon());
            if(currDist < closestDist)
            {
//                bool headingIsStart2End = this->isHeadingStart2EndOfCurrentRoad(nextRoad->value);
//                float distAlongRoad = this->getDistAlongRoad(nextRoad->value, false, headingIsStart2End);
//                
//                if(city->roadIsOnTrace(nextRoad->value, &this->tripLog, distAlongRoad / nextRoad->value->getSplineLength() - .1))
//                {
//                    closestDist = currDist;
//                    closestRoad = nextRoad->value;
//                }
                
                closestDist = currDist;
                closestRoad = nextRoad->value;
            }
            nextNode = roadNodes->nextEntry();
        }
        delete(nextNode);
        
        nextRoad = roads->nextEntry();
    }
    delete(nextRoad);
    
    return closestRoad;
}
    
Road* GPS::getCurrentRoad1(City* city)
{
    std::pair<double, double> latLon = this->readGPS();
    return this->getCurrentRoad2(city, latLon.first, latLon.second);
}
    
float GPS::getDistAlongRoad(Road* road, bool updateTripLog, bool headingIsStart2End)
{
    double lat;
    double lon;
    if(updateTripLog)
    {
        std::pair<double, double>* latLon = this->updateTripLog();
        lat = latLon->first;
        lon = latLon->second;
    }
    else
    {
        std::pair<double, double> latLon = this->readGPS();
        lat = latLon.first;
        lon = latLon.second;
    }
    
    GenericMap<long int, Node*>* nodes = road->getNodes();
    float distAlongRoad = 0.0;
    
    nodes->initializeCounter();
    GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
    
    double prevLat = nextNode->value->getLat();
    double prevLon = nextNode->value->getLon();
    
    nextNode = nodes->nextEntry();
    
    while(nextNode != NULL)
    {
        double currLat = nextNode->value->getLat();
        double currLon = nextNode->value->getLon();
        
        distAlongRoad += this->deltaLatLonToXY(prevLat, prevLon, currLat, currLon);
        
        // stop tracking distance once in proximity to current lat / lon
        if(this->deltaLatLonToXY(lat, lon, currLat, currLon) < this->deltaXYTolerance)
        {
            break;
        }
        
        prevLat = currLat;
        prevLon = currLon;

        nextNode = nodes->nextEntry();
    }
    delete(nextNode);
    
    Node* startNode = road->getNodes()->getEntry(0);

    Intersection* startInt = road->getStartIntersection();
    Intersection* endInt = road->getEndIntersection();
    
    float start2StartDist = this->deltaLatLonToXY(startNode->getLat(), startNode->getLon(), startInt->getLat(), startInt->getLon());
    float start2endDist = this->deltaLatLonToXY(startNode->getLat(), startNode->getLon(), endInt->getLat(), endInt->getLon());
    
    bool nodeIsStart2End = start2StartDist < start2endDist;
    
    if(headingIsStart2End != nodeIsStart2End)
    {
        distAlongRoad = road->getSplineLength() - distAlongRoad;
    }
    
    return distAlongRoad;
}
    
bool GPS::isHeadingStart2EndOfCurrentRoad(Road* road)
{
    // get lat lon
    std::pair<double, double> latLon = this->readGPS();
    double lat = latLon.first;
    double lon = latLon.second;
    
    // get road nodes
    GenericMap<long int, Node*>* nodes = road->getNodes();
    
    // iterate over road nodes
    nodes->initializeCounter();
    GenericEntry<long int, Node*>* nextNode = nodes->nextEntry();
    
    double prevLat = nextNode->value->getLat();
    double prevLon = nextNode->value->getLon();
    
    nextNode = nodes->nextEntry();
  
    double currLat = prevLat;
    double currLon = prevLon;
    
    while(nextNode != NULL)
    {
        currLat = nextNode->value->getLat();
        currLon = nextNode->value->getLon();
        
        // stop tracking distance once in proximity to current lat / lon
        if(this->deltaLatLonToXY(lat, lon, currLat, currLon) < this->deltaXYTolerance)
        {
            break;
        }
        
        // update prev
        prevLat = currLat;
        prevLon = currLon;
        
        nextNode = nodes->nextEntry();
    }
    delete(nextNode);
    
    double dLat = prevLat - currLat;
    double dLon = prevLon - currLon;
    
    // evaluate spline heading
    double angleSpline = std::atan2(dLat, dLon);
    angleSpline = this->boundTheta(angleSpline);
    
    // get gps heading
    double angleHeading = this->getHeadingAngle();
    
    // discern direction of travel relative to eval orientation of spline
    bool evalUp = false;
    float angleDiff = std::abs(angleHeading - angleSpline);
    if(angleDiff < M_PI / 4 || angleDiff > 2 * M_PI - M_PI / 4)
    {
        evalUp = true;
    }
    
    // determine where start and end intersections are located
    Node* endNodeWRTHeading;
    if(evalUp)
    {
        int nodeSize = road->getNodes()->getSize() - 1;
        endNodeWRTHeading = road->getNodes()->getEntry(nodeSize);
    }
    else
    {
        endNodeWRTHeading = road->getNodes()->getEntry(0);
    }
    
    double endLat = endNodeWRTHeading->getLat();
    double endLon = endNodeWRTHeading->getLon();
    
    Intersection* startInt = road->getStartIntersection();
    Intersection* endInt = road->getEndIntersection();
    
    float startDist = this->deltaLatLonToXY(endLat, endLon, startInt->getLat(), startInt->getLon());
    float endDist = this->deltaLatLonToXY(endLat, endLon, endInt->getLat(), endInt->getLon());
    
    return endDist < startDist;
}
    
double GPS::getHeadingAngle()
{
    std::pair<double, double> latLon1 = this->readGPS();
    std::pair<double, double> latLon2 = this->readGPS();
    
    double dLat = latLon1.first - latLon2.first;
    double dLon = latLon1.second - latLon2.second;
    
    double angle = std::atan2(dLat, dLon);
    angle = this->boundTheta(angle);
    
    return angle;
}
    
double GPS::boundTheta(double theta)
{
    while(theta < 0)
    {
        theta += 2 * M_PI;
    }
    
    while(theta > 2 * M_PI)
    {
        theta -= 2 * M_PI;
    }
    
    return theta;
}
    
double GPS::getDeltaXYTolerance()
{
    return this->deltaXYTolerance;
}
    
}
