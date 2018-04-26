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


int getData()
{
    int n2;
    if ( (n2 = recv(newsockfd, buffer, 256, 0) ) < 0 )
    {
        error( const_cast<char *>( "ERROR reading from socket") );
        return 0;
    }
    buffer[n2] = '\0';
    cout << "recv: " << buffer << endl;
    return atoi(buffer);
}


int main()
{
    // Socket setup to receive command strings from the PC
    // This has way more code, but is more efficient, as it's a standard inbuild socket type

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
     {
         //error( const_cast<char *>("ERROR opening socket") );
         cout << "Socket Error" << endl;
     }

     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons( portRecv );

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     {
       //error( const_cast<char *>( "ERROR on binding" ) );
       cout << "Bind Error" << endl;
     }

     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     // Wait for the PC to connect to the PI (TCP)
     connectToPC();

     // create ArduinoController
     const char * com = "/dev/ttyACM0";
     ArduinoController *droid = new ArduinoController(com);

    for (;;)
    {
        getData();
        // split up buffer on comma , delimeter
        char *pt;
        pt = strtok (buffer,",");
        int cmd[2];
        int k = 0;
        while (pt != NULL)
        {
            cmd[k] = atoi(pt);
            //printf("%d\n", cmd[k]);
            pt = strtok (NULL, ",");
            k++;
        }

        droid->step(cmd[0], cmd[1]);
        //droid->step(50, 100);
    }


    return 0;
}
