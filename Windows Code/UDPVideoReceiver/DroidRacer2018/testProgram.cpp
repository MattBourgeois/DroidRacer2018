#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <stdio.h>

#include <ctime>
#include <string>

//#define LOCAL

#ifdef LOCAL
	#include "MockUDPClientWrapper.h"
	#include "MockTCPControlSocket.h"
#else
	#include "TCPControlSocket.hpp"
	#include "UDPClientWrapper.hpp"
#endif
#include <windows.h>
#include "TrainingHandler.h"

#define LOG(x) std::cout << x << std::endl

/*
If LOCAL environment variable is active
program initializes MOCK versions of Client and Pi variables
This means no external connection is required
else if LOCAL is not defined
program initializes non-MOCK version of Client and Pi variables
An external connection is made to the droid
*/
#ifdef LOCAL
	//LOG("Running LOCAL processes");
	MockTCPControlSocket *Pi = new MockTCPControlSocket();
	MockUDPClientWrapper *Client = new MockUDPClientWrapper();
#else
	//LOG("Running GLOBAL processes"); 192.168.1.24
	TCPControlSocket *Pi = new TCPControlSocket("192.168.1.199", 5001);
	UDPClientWrapper *Client = new UDPClientWrapper("192.168.1.199", 65507, 5000);
#endif

int throttle = 0;	// 0 = stopped, 100 = moving (at the slowest possible speed, for now)
int angle = 75; // 30 to 130 degrees, defaults at 75 (straight)
int secondsToRecord = 120;

int angleSpeed = 10;

Mat frame;

void controls() {
	cvWaitKey(1);

	// some real basic controls - change to xbox gamepad? 
	if (GetAsyncKeyState(VK_UP) & 0x8000){
		// the 'Up' arrow key is currently being held down
		throttle = 101;
		printf("Forward pressed!\n");
	}else{
		throttle = 0;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
		// the 'Left' key is currently being held down
		if (angle > 40){
			angle -= angleSpeed;
			printf("Left pressed!\n");
		}
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000){
		// the 'Right' key is currently being held down
		if (angle < 120){
			angle += angleSpeed;
			printf("Right pressed!\n");
		}
	}
}

void step() {
	// Action the controls
	controls(); 

	// Send to pi
	std::string instruction = ":" + std::to_string(angle) + "," + std::to_string(throttle) + ".";
	std::string str;
	char * writable = new char[str.size() + 1];
	std::copy(instruction.begin(), instruction.end(), writable);
	writable[instruction.size()] = '\0';
	Pi->sendData(writable);
}


void training_run(std::string directory, std::string name, int seconds) {
	TrainingHandler training(directory, name);
	std::time_t startTime = std::time(0);
	std::time_t currentTime = std::time(0);

	while (currentTime - startTime < seconds) {
		step();
		frame = Client->receive();
		//imshow("output", frame);
		//waitKey(1);
		training.addSample(frame, angle, throttle);
		currentTime = std::time(0);
		
	}
	training.saveSamples();
}


int main(){
	Pi->setup();
	Client->setup();
	training_run("C:/Users/Matthew Lee/Desktop/training_data", "training.csv", secondsToRecord);
	return 0;
}
