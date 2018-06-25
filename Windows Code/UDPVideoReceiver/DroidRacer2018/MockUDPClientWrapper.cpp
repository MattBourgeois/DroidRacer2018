#include "MockUDPClientWrapper.h"
using namespace cv;


MockUDPClientWrapper::MockUDPClientWrapper(){
	this->stream = VideoCapture(0);
	this->frame = Mat();
}

Mat MockUDPClientWrapper::receive() {
	this->stream.read(this->frame);
	imshow("Video Feed", this->frame);
	return this->frame;
}

int MockUDPClientWrapper::setup() {
	/*
		Here's a haiku:
			This is a mock class
			There is nothing to set-up
			So this block is blank
	*/
	return 0;
}
