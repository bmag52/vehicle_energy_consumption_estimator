/*
 * VehicleDiagnostics.cpp
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#include "VehicleDiagnostics.h"

namespace PredictivePowertrain {
    
VehicleDiagnostics::VehicleDiagnostics()
{
    this->fd =  -1;
    this->timeMultiplierSFFEL = 3000;
    this->initializeDiagnosticsReader();
    
    this->vehicleSpeed = "01 0D\r";
    this->engineLoad = "01 04\r";
    this->airFlow = "01 10\r";
    this->o2Data = "01 24\r";
}

VehicleDiagnostics::~VehicleDiagnostics() {}

void VehicleDiagnostics::initializeDiagnosticsReader()
{
    this->fd = open("/dev/cu.wchusbserialfd120", O_RDWR | O_NONBLOCK);
    
    if(this->fd < 0)
    {
        std::cout << "Unable to open /dev/tty." << std::endl;
    }
    
    std::cout << "initializing vd" << std::endl;
    
    struct termios theTermios;
    
    memset(&theTermios, 0, sizeof(struct termios));
    cfmakeraw(&theTermios);
    cfsetspeed(&theTermios, 38400);
    
    // must stay in this particular order
    theTermios.c_cflag = CREAD | CLOCAL;      // turn on READ
    theTermios.c_cflag &= ~PARENB;            // Make 8n1
    theTermios.c_cflag &= ~CSTOPB;
    theTermios.c_cflag &= ~CSIZE;
    theTermios.c_cflag &= ~CRTSCTS;           // no flow control
    theTermios.c_cflag |= CS8;

    theTermios.c_cc[VMIN] = 0;
    theTermios.c_cc[VTIME] = 10;              // 1 sec timeout
    ioctl(this->fd, TIOCSETA, &theTermios);
    
    std::vector<std::string> startup(10);
    startup.at(0) = "AT D\r";
    startup.at(1) = "AT D\r";
    
    startup.at(2) = "AT I\r";
    startup.at(3) = "AT E0\r";
    startup.at(4) = "AT L1\r";
    startup.at(5) = "AT H0\r";
    startup.at(6) = "AT S1\r";
    startup.at(7) = "AT AL\r";

//    startup.at(2) = "AT Z\r";
//    startup.at(3) = "AT E0\r";
//    startup.at(4) = "AT L0\r";
//    startup.at(5) = "AT S0\r";
//    startup.at(6) = "AT H0\r";
    
    startup.at(8) = "AT SP 0\r";
    startup.at(9) = "0100\r";
    
    this->clearRxBuffer();
    
    for(int i = 0; i < startup.size() - 1; i++)
    {
        std::cout << this->getDiagnostsics(startup.at(i), 300) << std::endl;
    }
    
    // initialize
    std::cout << this->getDiagnostsics(startup.at(9), 100000) << std::endl;
    
    this->clearRxBuffer();
}

std::string VehicleDiagnostics::readDiagnostics()
{
    // define vars
    char buf[255];
    size_t res;
    
    while(true) // wait for read
    {
        res = read(this->fd,buf,255);
        
        if(res > 0)
        {
            buf[res]=0;
            
            std::string newMsg(buf);
            return newMsg;
            break;
        }
    }
}

float VehicleDiagnostics::getSpeed()
{
    std::string vehSpdRaw = this->getDiagnostsics(this->vehicleSpeed, this->timeMultiplierSFFEL);
    
    // get val
    float spdkph = this->hex2Float(vehSpdRaw.substr(6,2));
    
    return spdkph / 3.6;
}

float VehicleDiagnostics::getFuelFlow()
{
    float afr_i = this->readO2();
    float airFlow_i = this->readMAF();
    float fuelFlow = airFlow_i / afr_i;
    
    std::cout << "fuel flow: " << fuelFlow << std::endl;
    
    return fuelFlow;
}
 
float VehicleDiagnostics::readMAF()
{
    std::string airFlowRaw = this->getDiagnostsics(this->airFlow, this->timeMultiplierSFFEL);
    
    // parse air float
    float AF_A = this->hex2Float(airFlowRaw.substr(6,2));
    float AF_B = this->hex2Float(airFlowRaw.substr(9,2));
    float MAF = (256.0 * AF_A + AF_B) / 4;
    
    std::cout << "MAF: " << MAF << std::endl;
    
    return MAF;
}
    
float VehicleDiagnostics::readO2()
{
    std::string o2DataRaw_i = this->getDiagnostsics(this->o2Data, this->timeMultiplierSFFEL);
    
    float o2Data_A = this->hex2Float(o2DataRaw_i.substr(6,2));
    float o2Data_B = this->hex2Float(o2DataRaw_i.substr(9,2));
    float AFR = 2.0 / 65526.0 * (256.0 * o2Data_A + o2Data_B);
    
    std::cout << "AFR: " << AFR << std::endl;
    
    return AFR;
}
    
float VehicleDiagnostics::getEngineLoad()
{
    std::string engineLoadRaw = this->getDiagnostsics(this->engineLoad, this->timeMultiplierSFFEL);
    
    float engineLoad_A = this->hex2Float(engineLoadRaw.substr(6,2));
    float engineLoad = engineLoad_A / 2.55;
    
    std::cout << "engine load: " << engineLoad << std::endl;
    
    return engineLoad;
}
    
std::string VehicleDiagnostics::getDiagnostsics(std::string cmd, int timeMultiplier)
{
    if(this->fd < 0)
    {
        this->initializeDiagnosticsReader();
    }
    
    this->clearRxBuffer();
    
    write(this->fd, cmd.c_str(), cmd.length());
    
    usleep((25 * cmd.length()) * timeMultiplier);
    
    std::string response = this->readDiagnostics();
    
    if(!response.find("NO DATA"))
    {
        response = "000000000000000";
    }
    
    return response;
}
    
float VehicleDiagnostics::hex2Float(std::string hex)
{
    if(hex.find("0x") == std::string::npos)
    {
        hex = "0x" + hex;
    }
    
    float val;
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> val;
    return val;
}

void VehicleDiagnostics::clearRxBuffer()
{
    // clear buffer remainder
    char buf[255];
    int i = 1;
    while(i > 0)
    {
        i = read(this->fd, buf, 255);
        std::string msg(buf);
    }
}
    
}
