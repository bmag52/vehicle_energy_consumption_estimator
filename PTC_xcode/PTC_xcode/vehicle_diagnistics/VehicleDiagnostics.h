/*
 * VehicleDiagnostics.h
 *
 *  Created on: Apr 17, 2016
 *      Author: vagrant
 */

#ifndef VEHICLE_DIAGNOSTICS_GPS_H_
#define VEHICLE_DIAGNOSTICS_GPS_H_

#include <utility>
#include <cmath>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <vector>

#include "../map/GenericMap.h"

namespace PredictivePowertrain {
        
    class VehicleDiagnostics {
    private:
        
        int fd;
        int timeMultiplierSFFEL;
        
        std::string vehicleSpeed;
        std::string engineLoad;
        std::string airFlow;
        std::string o2Data;
        
        float readO2();
        float readMAF();
        void initializeDiagnosticsReader();
        std::string readDiagnostics();
        std::string getDiagnostsics(std::string cmd, int timeMultiplier);
        
        void clearRxBuffer();
        float hex2Float(std::string hex);
        
    public:
        
        VehicleDiagnostics();
        virtual ~VehicleDiagnostics();
        
        float getSpeed();
        float getFuelFlow();
        float getEngineLoad();
    };
    
}



#endif /* VEHICLE_DIAGNOSTICS_GPS_H_ */
