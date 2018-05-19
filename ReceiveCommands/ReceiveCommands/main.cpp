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

int clearBuffer()
{

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
     //std::fstream arduino;
     //arduino.open(com);

        //droid->step(90,90);
        sleep(3);
    //droid->step(75,0);
/*
    printf("\nbuffer: %s\n", buffer);
    for (int j = 0; j < 256; j++)
    {
        buffer[j]=0;
    }
*/
    const char * com1 = "/dev/ttyACM0";
     const char * com2 = "/dev/ttyACM1";
     fstream a1;
     fstream a2;
     const char * ctrl1 = "10, 10";
     const char * ctrl2 = "130, 130";
     bool alternate = true;
     sleep(1);

    for (;;)
    {
        //usleep(150000);
        // Get that sweet, sweet data bruh
        getData();
        string str(buffer);
        for (int j = 0; j < 256; j++)
        {
            char a = '\0';
            buffer[j]= a;
        }
        string buf(buffer);
        cout << "BufferClear: " << buf << endl;
        // split up buffer on comma , delimeter

        //cout << buffer <<endl;
        string start;
        cout << "Buffer: " << str << endl;
        if (str.length() >20)
        {
            start= str.substr(str.find(":", str.length()-20));
        }else{
            start = str;
        }
        string command = start.substr(0, start.find("."));
        string power = command.substr(1, command.find(",")-1);
        string angle = command.substr(command.find(",")+1,command.find(".")-1);
        cout<< "String: " << start << endl;
        cout << "StringLen: " << str.length() << endl;
        cout << "Command" << command << endl;
        cout << "Power: " << power << endl;
        cout << "Angle: " << angle << endl;
        int a = stoi(angle);
        int p = stoi(power);

        char *cstr = &command[0u];
        cout<<"CSTR"<<cstr <<endl;

        //->step(cmd[0], cmd[1]);
        a1.open(com1);
        a2.open(com2);
        usleep(100000);
        a1 << cstr << endl;
        a2 << cstr << endl;
    /*
        if(alternate){
            a1 << ctrl1 << endl;
            a2 << ctrl1 << endl;
        }else{
            a1 << ctrl2 << endl;
            a2 << ctrl2 << endl;
        }
1*/
        alternate = !alternate;
        a1.close();
        a2.close();
        //usleep(150000);

        //arduino << droid->step(a, p) <<endl;
       //arduino << command << endl;

       //arduin << droid->step(a, p) <<endl;
        //droid->step(30,101);
        //droi->step(30,101);
        //sleep(3);
        //droid->step(130,0);
        //droi->step(130,0);

        /*
        arduino << droid->step(30, 0) << endl;
        usleep(1000000);
        arduino << droid->step(130, 0) << endl;
        usleep(1000000);
        //droid->step(130, 0);
        //droid->step(30, 101);
*/
        //printf("\nbuffer: %s\n", buffer);




    }


    return 0;
}
