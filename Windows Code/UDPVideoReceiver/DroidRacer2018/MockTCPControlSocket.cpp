#include "MockTCPControlSocket.h"

MockTCPControlSocket::MockTCPControlSocket()
{
	/*
		THIS IS A MOCK FUNCTION
		This would normally init connection to the droid
		But in testing we don't want to do that
		So we do nothing here.
	*/
}

void MockTCPControlSocket::sendData(char * buffer)
{
	/*
		THIS IS A MOCK FUNCTION
		This would normally send information to the droid
		But in testing we don't want to do that
		So we do nothing here.
	*/
}

int MockTCPControlSocket::setup()
{
	/*
		THIS IS A MOCK FUNCTION
		This would make a connection to the droid.
		But this is a mock. So it does nothing.
	*/
	return 0;
}
