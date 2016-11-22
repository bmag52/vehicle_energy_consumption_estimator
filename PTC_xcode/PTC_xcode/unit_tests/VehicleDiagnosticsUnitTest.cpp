/*
 *  VehicleDiagnostics
 *
 *
 */

#include "../vehicle_diagnistics/VehicleDiagnostics.h"
#include <chrono>

using namespace PredictivePowertrain;

// unit test for the SpeedPrediction class
void vehicleDiagnostics_ut()
{
    VehicleDiagnostics vd;
    
    // log values for duration of time
    int timeItr = 0;
    while(timeItr < 360)
    {
        std::cout << "------ " << timeItr << " ------" << std::endl;
        std::cout << "fuel flow (g/a): " << vd.getFuelFlow() << std::endl;
        std::cout << "speed (mph): " << vd.getSpeed() * 2.23 << std::endl;
        std::cout << "engine load (%): " << vd.getEngineLoad() << std::endl;
        
//        auto start = std::chrono::system_clock::now();
//        while(1)
//        {
//            auto end = std::chrono::system_clock::now();
//            std::chrono::duration<double> diff = end - start;
//            
//            if(diff.count() > 1.0) { break; }
//        }
        
        timeItr++;
    }
}
