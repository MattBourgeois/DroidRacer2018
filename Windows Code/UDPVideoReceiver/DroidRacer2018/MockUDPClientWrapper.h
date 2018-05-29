//#include "UDPClient.hpp"
#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <conio.h>

using namespace cv;

class MockUDPClientWrapper
{
private:
	VideoCapture stream;
	Mat frame;
public:
	MockUDPClientWrapper();
	Mat receive();
};