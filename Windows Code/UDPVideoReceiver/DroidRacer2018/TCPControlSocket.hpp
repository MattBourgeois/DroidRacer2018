#include <winsock2.h> // moved these libraries to stafx.h
#include <stdio.h>
//#pragma comment(lib, "ws2_32.lib")

class TCPControlSocket
{
public:
	// ToDo: put this in a wrapper class
	// variable declarations
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;

	// constructor
	TCPControlSocket(const char *inAddr, int inPort)
	{
		server.sin_addr.s_addr = inet_addr(inAddr); // the IP address of the server you wish to connect to
		server.sin_family = AF_INET;
		server.sin_port = htons(inPort); // the port your service is using
	}

	// setup
	int setup()
	{
		// initialise winsock library
		
		printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Initialisation failed. Error Code : %d", WSAGetLastError());
			return 1;
		}
		printf("Initialised.\n");

		//Create a socket, s
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
		}
		printf("Socket created.\n");

		// Define socket properties
		//server.sin_addr.s_addr = inet_addr("169.254.28.95"); // the IP address of the server you wish to connect to
		//server.sin_family = AF_INET;
		//server.sin_port = htons(5001); // the port your service is using

									   //Connect to remote server
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			printf("Connection error");
			return 1;
		}
		printf("Connected \n");
	}

	int sendData(const char *message)
	{
		if (send(s, message, strlen(message), 0) <= 0)
		{
			printf("Send failed \n");
			return 1;
		}
		printf("Data Sent\n");
	}
};