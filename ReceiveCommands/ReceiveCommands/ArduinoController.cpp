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
    //std::fstream arduino;
    //arduino.open(port);
}

char* ArduinoController::step(int steeringAngle, int throttle) {
    this->update(steeringAngle, throttle);

    //this->arduino = fopen(this->port
	printf("PORT: ");
	printf(port);
	printf("\n");

    if(this->arduino < 0){
        printf("Failed");
        //return false;
    }else{
        printf("Success :");
    }
	printf(this->instructions);
	printf("\n");
	return this->instructions;
    //arduino << this->instructions << std::endl;
    //arduino.close();
    //return true;
}
