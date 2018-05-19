#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <zconf.h>
#include "ArduinoController.h"
#include "ArduinoController.cpp"

using namespace std;

//Filthy Global Data
int sockfd, newsockfd, portRecv = 5001, clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr, n, data;

void error( char *msg ) {
  perror(  msg );
  exit(1);
}

//Initial Connection to PC
void connectToPC()
{
     while (1)
     {
        printf( "waiting for PC to connect\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
        {
            error( const_cast<char *>("ERROR on accept") );
        }
        else
        {
            printf( "PC Client connected!\n" );
            break;
        }
	 }
}

//Retreive data from socket.
int getData()
{
    int n2;
    if ( (n2 = recv(newsockfd, buffer, 256, 0) ) < 0 )
    {
        error( const_cast<char *>( "ERROR reading from socket") );
        return 0;
    }
    buffer[n2] = '\0';
    return atoi(buffer);
}

int main()
{
    // Socket setup to receive command strings from the PC
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "Socket Error" << endl;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portRecv );

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "Bind Error" << endl;
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    // Wait for the PC to connect to the PI (TCP)
    connectToPC();
    sleep(3); //Allow time to connect

    //Create filestreams to both comports
    const char * com1 = "/dev/ttyACM0";
    const char * com2 = "/dev/ttyACM1";
    fstream a1;
    fstream a2;
    sleep(2);

    for (;;)
    {
        // Get that sweet, sweet data bruh
        getData();
        string str(buffer);
        cout << "Buffer: " << str << endl;

        //Clear buffer
        for (int j = 0; j < 256; j++)
        {
            char a = '\0';
            buffer[j]= a;
        }

        //Find start of a more recent command.
        string start;
        if (str.length() >20)
        {
            start= str.substr(str.find(":", str.length()-20));
        }else{
            start = str;
        }

        //Gather entire command
        string command = start.substr(0, start.find("."));
        cout << "Command" << command << endl;

        //Convert command string to cString
        char *cstr = &command[0u];
        cout<<"CSTR"<<cstr <<endl;

        //Send command to Arduino
        a1.open(com1);
        a2.open(com2);
        usleep(100000);
        a1 << cstr << endl;
        a2 << cstr << endl;
        a1.close();
        a2.close();
    }
    return 0;
}
