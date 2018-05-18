#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include <iostream>
#include "protocols/basic_protocol.h"
#include "sender/sender_socket.h"
#include "util/util.h"
#include "video/video_capture.h"

using udp_streaming_video::BasicProtocolData;
using udp_streaming_video::SenderSocket;
using udp_streaming_video::VideoCapture;

using namespace cv;
using namespace std;

int main()
{
    const int port = 5000;
    if (port < 0) {
    	return -1;
    }
    std::string ip_address = "192.168.43.113";  // Localhost
    const SenderSocket socket(ip_address, port);
    std::cout << "Sending to " << ip_address << " on port " << port << "." << std::endl;

    BasicProtocolData protocol_data;
	udp_streaming_video::VideoCapture video_capture(false, 1);

    for (;;)
    {
		protocol_data.SetImage(video_capture.GetFrameFromCamera());
		socket.SendPacket(protocol_data.PackageData());
		//waitKey(1);
    }
    cout << "Hello world!" << endl;
    return 0;
}
