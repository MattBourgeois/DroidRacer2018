//=========================================
// Created by Matthew Lee on 8/04/18.
//=========================================

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "ArduinoController.h"
#include <fstream>

ArduinoController::ArduinoController(const char *port) {
    /**
     * @param const char *port: The serial port to connect to the arduino (e.g /dev/ttyACM1)
     */
    this->port = port;
}

bool ArduinoController::step(int steeringAngle, int throttle) {
    this->update(steeringAngle, throttle);
    std::fstream arduino;
    arduino.open("/dev/ttyACM0");

    if(this->arduino < 0){
        printf("Failed");
        return false;
    }else{
        printf("Success");
    }
    arduino << this->instructions << std::endl;
    arduino.close();
    return true;
}
