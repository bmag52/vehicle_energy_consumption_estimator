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
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>

namespace PredictivePowertrain {
        
    class VehicleDiagnostics {
    private:
        
        int fd;
        
        void initializeDiagnosticsReader();
        
    public:
        
        VehicleDiagnostics();
        virtual ~VehicleDiagnostics();
        
        float readDiagnostics();
        float getSpeed();
        float getFuelFlow();
        float getFuelPressure();
    };
    
}



#endif /* VEHICLE_DIAGNOSTICS_GPS_H_ */
