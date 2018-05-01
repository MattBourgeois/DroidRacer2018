//=========================================
// Griffith Innovate 2018
// Droid Racer Competition
// Created by Matthew Lee on 8/04/18.
//=========================================

#ifndef DROID_RACER_ENGINECONTROLLER_H
#define DROID_RACER_ENGINECONTROLLER_H


class EngineController {
protected:
    int MAXANGLE = 130;
    int MINANGLE = 30;
    int MAXTHROTTLE = 130;
    int MINTHROTTLE = 100;
private:
    int steeringAngle = 0.0;
    int throttle = 0.0;
    void updateInstructions();
public:
    char instructions [10];
    int setSteeringAngle(int steeringAngle);
    int setThrottle(int throttle);
    int getSteeringAngle();
    int getThrottle();
    virtual bool update(int steeringAngle, int throttle);
};


#endif //DROID_RACER_ENGINECONTROLLER_H
