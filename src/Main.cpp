
#include "VlcCapture.h"

#include <unistd.h>
#include <iostream>



int main(int argc, char** argv){
	
	std::string url;

	if (argc ==2){
		url = argv[1];
	}else{
		url="rtsp://localhost:8554/vid.mpg";
	}

	VlcCapture cap(url);

	cv::Mat image;// = cv::Mat(1080,1920,CV_8UC3);

	cap.start();

	while (cv::waitKey(20) != 27){
		if (cap.read(image)){
			cv::imshow("image",image);
		}else{
			usleep(10000);
		}
	}

	cap.stop();

}
