#include "UDPClient.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <conio.h>

#define CV_H

using namespace std;
using namespace cv;

// This is a wrapper class for the webcam UDP stream client
// It has a constructor, and 2 functions
// Constructor: 
//		Pass the folliwing variables to the constructor: 
//		Addr	-> IP address of the raspberry pi
//		UDPMAX	-> The size of each frame? Default 65507
//		port	-> The port number of this service, default 5000
// wrapperSetup		-> probably not required, makes an openCV window, but this could potentially be moved to the constructor? 
// wrapperReceive	-> receives each frame, reconstructs it as a jpeg, and displays this in the openCV window created in setup()

class UDPClientWrapper
{
public:
	const char *addr;
	int UDPMAX;
	int port;
	UDPClient *client;
	char *buff;
	vector<uchar> videoBuffer;

	// Constructor
	UDPClientWrapper(const char *inAddr, int inUDPMAX, int inPort)
	{
		addr = inAddr;
		UDPMAX = inUDPMAX;
		port = inPort;
		client = new UDPClient(port);
		buff = (char*)malloc(UDPMAX);
	}

	void setup()
	{
		cout << "=== VIDEO RECEIVER ===" << endl;
		//setup openCV
		cvNamedWindow("UDP Video Receiver", CV_WINDOW_AUTOSIZE);
	}

	Mat receive()
	{
		//read data
		int result = client->receiveData(buff, UDPMAX);
		if (result <= 0)
		{
			cout << "Failed to receive frame. UDPClientData::receiveData() returned code:  " << result << endl;
		}
		else
		{
			cout << "Got a frame of size " << result << endl;

			videoBuffer.resize(result);
			memcpy((char*)(&videoBuffer[0]), buff, result);

			//reconstruct jpeg and display it
			Mat jpegimage = imdecode(Mat(videoBuffer), CV_LOAD_IMAGE_COLOR);
			IplImage img = jpegimage;
			//cvShowImage("UDP Video Receiver", &img);
			// Return OpenCV Matrix
			return jpegimage;
			//cvWaitKey(1);
		}
	}

	int wrapperReceive(IplImage img)
	{
		//read data
		int result = client->receiveData(buff, UDPMAX);
		if (result <= 0)
		{
			cout << "Failed to receive frame. UDPClientData::receiveData() returned code:  " << result << endl;
		}
		else
		{
			cout << "Got a frame of size " << result << endl;

			videoBuffer.resize(result);
			memcpy((char*)(&videoBuffer[0]), buff, result);

			//reconstruct jpeg and display it
			Mat jpegimage = imdecode(Mat(videoBuffer), CV_LOAD_IMAGE_COLOR);
			IplImage img = jpegimage;
			//cvShowImage("UDP Video Receiver", &img);
		}
	}
};
