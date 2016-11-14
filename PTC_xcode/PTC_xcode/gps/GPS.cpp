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
    std::cout << latLonRef.first << "," << latLonRef.second << std::endl;
    std::pair<double, double>* latLonPtr = new std::pair<double, double>(latLonRef.first, latLonRef.second);
	this->tripLog.addEntry(this->tripCount, latLonPtr);
    this->tripCount++;
    return latLonPtr;
}

GenericMap<long int, std::pair<double, double>*>* GPS::getTripLog() {
	return &this->tripLog;
}
    
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
    return radians / (2* M_PI) * 360.0;
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
                else{
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
    
    if(!headingIsStart2End)
    {
        distAlongRoad = road->getSplineLength() - distAlongRoad;
    }
    
    return distAlongRoad;
}
    
bool GPS::isHeadingStart2EndOfCurrentRoad(Road* road)
{
    // assume road is current road
    float distAlongRoad = this->getDistAlongRoad(road, false, true);
    float evalSVal1 = distAlongRoad / road->getSplineLength();
    float evalSVal2 = (distAlongRoad + 5.0) / road->getSplineLength();
    
    // cap eval2 s-value at 1.0
    evalSVal2 = std::max((float)1.0, evalSVal2);
    
    Eigen::Spline<double,2>::PointType pt1 = road->getSpline()(evalSVal1);
    Eigen::Spline<double,2>::PointType pt2 = road->getSpline()(evalSVal2);
    
    double dLat = pt1(0,0) - pt2(0,0);
    double dLon = pt1(0,1) - pt2(0,1);
    
    double angleSpline = std::atan2(dLat, dLon);
    if(angleSpline < 0)
    {
        angleSpline += M_PI;
    }
    
    double angleHeading = this->getHeadingAngle();
    
    if(std::abs(angleHeading - angleSpline) < M_PI / 2)
    {
        return true;
    }
    return false;
}
    
double GPS::getHeadingAngle()
{
    std::pair<double, double> latLon1 = this->readGPS();
    std::pair<double, double> latLon2 = this->readGPS();
    
    double dLat = latLon1.first - latLon2.first;
    double dLon = latLon1.second - latLon2.second;
    
    double angle = std::atan2(dLat, dLon);
    if(angle < 0)
    {
        angle += M_PI;
    }
    
    return angle;
}
    
}
