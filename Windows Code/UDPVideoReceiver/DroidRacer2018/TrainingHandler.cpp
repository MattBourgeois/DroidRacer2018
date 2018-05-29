#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "TrainingHandler.h"

using namespace std;



TrainingHandler::TrainingHandler(std::string directory, std::string filename) {
	this->filename = filename;
	if (directory.back() != '/') {
		directory.append("/");
	}
	std::cout << directory << std::endl;
	this->directory = directory;	
}

bool TrainingHandler::addSample(cv::Mat image, float angle, float throttle) {
	this->images.push_back(image);
	this->angles.push_back(angle);
	this->throttles.push_back(throttle);
	return 1;
}

bool TrainingHandler::saveSamples() {
	std::ofstream outFile;
	outFile.open(this->directory + this->filename);
	outFile << "image, angle, throttle\n";
	std::string imageName;
	for (int i = 0; i < this->images.size(); i++) {
		imageName = this->directory + "image_" + std::to_string(i) + ".png";
		cv::imwrite(imageName, this->images.at(i));
		outFile << imageName + "," + \
			std::to_string(this->angles.at(i)) + "," + \
			std::to_string(this->throttles.at(i)) + "\n";
	}
	outFile.close();
	return 1;
}