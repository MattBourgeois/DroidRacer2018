#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

class TrainingHandler {
private:
	std::string directory;
	std::string filename;
	std::vector<cv::Mat> images;
	std::vector<float> angles;
	std::vector<float> throttles;

public:
	TrainingHandler(std::string directory, std::string filename);
	bool addSample(cv::Mat image, float angle, float throttle);
	bool saveSamples();
};
