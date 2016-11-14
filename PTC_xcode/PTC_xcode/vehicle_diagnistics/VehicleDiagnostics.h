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
        
        GenericMap<int, std::pair<float, float>*> fuelFlowParams;
        int logCount = 0;
        
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
        
    public:
        
        VehicleDiagnostics();
        virtual ~VehicleDiagnostics();
        
        float getSpeed();
        float getFuelFlow();
        float getEngineLoad();
        float hex2Float(std::string hex);
        
    };
    
}



#endif /* VEHICLE_DIAGNOSTICS_GPS_H_ */
