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

void GPS::updateTripLog() {
	std::pair<double, double> latLonRef = this->readGPS();
    std::pair<double, double>* latLonPtr = new std::pair<double, double>(latLonRef.first, latLonRef.second);
	this->tripLog.addEntry(this->tripCount, latLonPtr);
    this->tripCount++;
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
                    
                    double lat = std::stod(latString) / 100;
                    double lon = - std::stod(lonString) / 100;
                    
                    //printf("%.6f,%.6f\n", lat, lon);
                    std::pair<double, double> latLon(lat, lon);
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
    
std::pair<bool, std::pair<double, double>> GPS::isOnRoad(Road* road)
{
    std::pair<double, double> latLon = this->readGPS();
    
    road->getNodes()->initializeCounter();
    GenericEntry<long int, Node*>* nextNode = road->getNodes()->nextEntry();
    while(nextNode != NULL)
    {
        if(this->deltaLatLonToXY(latLon.first, latLon.second, nextNode->value->getLat(), nextNode->value->getLon()) < this->deltaXYTolerance)
        {
            std::pair<bool, std::pair<double, double>> truePosition(true, latLon);
            return truePosition;
        }
        nextNode = road->getNodes()->nextEntry();
    }
    delete(nextNode);
    
    std::pair<bool, std::pair<double, double>> falsePosistion(false, latLon);
    return falsePosistion;

}

std::pair<bool, std::pair<double, double>> GPS::isAtIntersection(Intersection* intersection)
{
    std::pair<double, double> latLon = this->readGPS();
    
    if(this->deltaLatLonToXY(latLon.first, latLon.second, intersection->getLat(), intersection->getLon()) < this->deltaXYTolerance)
    {
        std::pair<bool, std::pair<double, double>> truePosition(true, latLon);
        return truePosition;
    }
    
    else
    {
        std::pair<bool, std::pair<double, double>> falsePosistion(false, latLon);
        return falsePosistion;
    }
}
    
}
