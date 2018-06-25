#pragma once
class MockTCPControlSocket
{
public:
	MockTCPControlSocket();
	void sendData(char* buffer);
	int setup();
};

