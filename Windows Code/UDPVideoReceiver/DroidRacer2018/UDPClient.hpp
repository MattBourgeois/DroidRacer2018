#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>

#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#pragma comment(lib, "Ws2_32.lib")


//CLIENT: UDP
class UDPClient {
	SOCKET clientSock;
	sockaddr_in default_destination;
	int default_destination_size;
	sockaddr_in last_address; //last address received from
	bool default_destination_set;
	WSADATA wsaData;
public:
	UDPClient();
	UDPClient(int local_port);
	UDPClient(char* host_addr, int host_port);
	UDPClient(SOCKET sock);
	~UDPClient();

	//establishing client:
	void setDefaultDestination(char* host, int port);
	void setDefaultDestinationToLastReceived(); //TODO - fix this
	int bindTo(int local_port);

	//send data:
	int sendData(char* buffer, int len); //send data to default host
	int sendData(char* buffer, int len, char* host, int port); //send data to
	int sendString(char* string); //send string to default host
	int sendString(char* string, char* host, int port); //send to host
	int sendInt(int i); //send int to default host
	int sendInt(int i, char* host, int port); //send to host
	int reply(char* buffer, int len);

	//receive data:
	int receiveData(char* buffer, int len);
	char* recvLine();
	int recvInt();

	//meta info:
	//char* getDestinationAddr();
	//int getDestinationPort();
	char* getLocalAddr();
	int getLocalPort();
};
#endif