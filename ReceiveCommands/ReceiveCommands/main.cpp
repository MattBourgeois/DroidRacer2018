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
    //cout << "recv: " << buffer << endl;
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
     const char * com = "/dev/ttyACM1";
     ArduinoController *droid = new ArduinoController(com);
        //droid->step(90,90);
        //sleep(3);
    //droid->step(75,0);

    printf("\nbuffer: %s\n", buffer);
    for (int j = 0; j < 256; j++)
    {
        buffer[j]=0;
    }


    for (;;)
    {
        // Get that sweet, sweet data bruh
        getData();

        // split up buffer on comma , delimeter
        string str(buffer);


        string cmd1 = str.substr(0, str.find(','));
        cout << "CMD1: " << cmd1 << endl;
        string test = str.erase(0, (cmd1.size() + 1));
        cout << "str after erase" << test << endl;
        string cmd2 = test.substr(0, test.find(','));
        cout << "CMD2 before: " << cmd2 << endl;
        int angle = stoi(cmd2);
        int throttle = stoi(cmd1);


        //cout << "str1: " << cmd1 << " str2: " << cmd2;

        cout << "cmd2: " << angle << " cmd1: " << throttle << endl;

        // split up the buffer by the comma, in C:
        /*for (int j = 0; j < 256; j++)
        {
            char temp1[k];
            char temp2[k];
            if (buffer[j] == '\0');
            {
                break;
            }
            if (buffer[j] == ',');
            {
                char temp[10];
                int k;
                for ( k = 0; k < j; k++)
                {
                    temp[k] = buffer[k];
                }
                temp[j] = '\0';
                continue;
            }

        }*/





        //cout << "Angle: " << cmd[0] << " Power: " << cmd[1] << endl;

        //->step(cmd[0], cmd[1]);
        usleep(10000);
        droid->step(angle, throttle);

        printf("\nbuffer: %s\n", buffer);
        for (int j = 0; j < 256; j++)
        {
            char a = '\0';
            buffer[j]= a;
        }


    }


    return 0;
}
