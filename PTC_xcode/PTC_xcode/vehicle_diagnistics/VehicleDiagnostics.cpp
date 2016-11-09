/*
 * VehicleDiagnostics.cpp
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#include "VehicleDiagnostics.h"

namespace PredictivePowertrain {
    
VehicleDiagnostics::VehicleDiagnostics() {}

VehicleDiagnostics::~VehicleDiagnostics() {}

void VehicleDiagnostics::initializeDiagnosticsReader()
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

float VehicleDiagnostics::readDiagnostics()
{
    if(this->fd < 0)
    {
        this->initializeDiagnosticsReader();
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
                    
                    return lat;
                    break;
                }
                else{
                    break;
                }
            }
        }
    }
}

float VehicleDiagnostics::getSpeed()
{
    return 0;
}

float VehicleDiagnostics::getFuelFlow()
{
    return 0;
}

float VehicleDiagnostics::getFuelPressure()
{
    return 0;
}
    
}
