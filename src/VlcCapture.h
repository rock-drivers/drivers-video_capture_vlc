/*
 * VlcCapture.h
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#ifndef VLCCAPTURE_H_
#define VLCCAPTURE_H_

// https://stackoverflow.com/questions/32825363/working-with-vlc-smem

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <pthread.h>

#include <vlc/vlc.h>

#include <opencv2/opencv.hpp>

void* lock_frame(void* user_data, void** planes);
void unlock_frame(void *user_data, void *picture, void *const *planes);
unsigned format_callback(void**user_data_ptr, char*chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines);

class VlcCaptureConsumer{
    public:
    /*
     * get called when a new image is arrived
     */
    virtual void imageCallback()=0;
};

class VlcCapture {
public:
	VlcCapture(std::string url = "", int input_buffer_ms = 1000);
	virtual ~VlcCapture();

	void open(std::string &url);
	bool read(cv::Mat &image);
	int start();
	void stop();

	pthread_mutex_t imagemutex;
	int width,height;
	unsigned char* buffer;
	unsigned int buffersize;
	bool imageAvailable;
	std::vector<VlcCaptureConsumer*> callbacks;
private:
	int buffer_ms;
    libvlc_instance_t *vlc;
	libvlc_media_t *vlcm;
    libvlc_media_player_t *vlcmp;
};

#endif /* VLCTOMEMORY_H_ */
