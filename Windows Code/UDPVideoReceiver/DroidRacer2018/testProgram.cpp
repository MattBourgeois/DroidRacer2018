#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>

#ifndef LOCAL
	#include "MockUDPClientWrapper.h"
	#include "MockTCPControlSocket.h"
#else
	#include "UDPClientWrapper.hpp"
	#include "TCPControlSocket.hpp"
#endif

#define LOG(x) std::cout << x << std::endl


	

int main()
{
	/*
		If LOCAL environment variable is active
			program initializes MOCK versions of Client and Pi variables
			This means no external connection is required
		else if LOCAL is not defined
			program initializes non-MOCK version of Client and Pi variables
			An external connection is made to the droid
	*/
	#ifndef LOCAL
		LOG("Running LOCAL processes");
		MockTCPControlSocket *Pi = new MockTCPControlSocket();
		MockUDPClientWrapper *Client = new MockUDPClientWrapper();
	#else
		LOG("Running GLOBAL processes");
		TCPControlSocket *Pi = new TCPControlSocket("192.168.43.175", 5001);
		Pi->setup();

		UDPClientWrapper *Client = new UDPClientWrapper("192.168.43.175", 65507, 5000);
		Client->setup();
	#endif

	boolean change = false;
	

	int motion = 0;	// 0 = stopped, 100 = moving (at the slowest possible speed, for now)
	int angle = 75; // 30 to 130 degrees, defaults at 75 (straight)

	while (1)
	{
		Mat frame = Client->receive();
		cvWaitKey(1);

		// some real basic controls - change to xbox gamepad? 
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			// the 'Up' arrow key is currently being held down
			motion = 101;
			printf("Forward pressed!\n");
		}
		else
		{
			motion = 0;
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			// the 'Left' key is currently being held down
			if (angle > 40)
			{
				angle-=10;
				printf("Left pressed!\n");
			}
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{

			// the 'Right' key is currently being held down
			if (angle < 120)
			{
				angle+= 10;
				printf("Right pressed!\n");
			}
		}

		std::string instruction = ":" + std::to_string(angle) + "," + std::to_string(motion) + ".";
		std::string str;
		char * writable = new char[str.size() + 1];
		std::copy(instruction.begin(), instruction.end(), writable);

		writable[instruction.size()] = '\0';
		Pi->sendData(writable);

	}

	return 0;
}
