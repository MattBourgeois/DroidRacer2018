#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include "UDPClientWrapper.hpp"
#include "TCPControlSocket.hpp"
#include <windows.h>


int main()
{

	TCPControlSocket *Pi = new TCPControlSocket("169.254.28.95", 5001);
	Pi->setup();

	UDPClientWrapper *Client = new UDPClientWrapper("169.254.28.95", 65507, 5000);
	Client->setup();

	int motion = 0;	// 0 = stopped, 10 = moving (at the slowest possible speed, for now)
	int angle = 90; // 0 to 180 degrees, defaults at 90 (straight)

	while (1)
	{
		Client->receive();
		cvWaitKey(1);

		// some real basic controls - change to xbox gamepad? 
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			// the 'Up' arrow key is currently being held down
			motion = 10;
			printf("Forward pressed!\n");
		}
		else
		{
			motion = 0;
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			// the 'Left' key is currently being held down
			if (angle >= 0)
			{
				angle--;
				printf("Left pressed!\n");
			}
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{

			// the 'Right' key is currently being held down
			if (angle <= 180)
			{
				angle++;
				printf("Right pressed!\n");
			}
		}
		std::string instruction = std::to_string(motion) + "," + std::to_string(angle);
		std::string str;
		char * writable = new char[str.size() + 1];
		std::copy(instruction.begin(), instruction.end(), writable);
		writable[instruction.size()] = '\0';
		Pi->sendData(writable);
	}

	return 0;
}

