/*
 * VlcCapture.cpp
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#include "VlcCapture.h"
#include <unistd.h>

VlcCapture::VlcCapture(std::string url){

	pthread_mutex_init(&imagemutex,NULL);// = PTHREAD_MUTEX_INITIALIZER;

	buffersize = 640*480*4;
	buffer = (unsigned char*)malloc(buffersize);
	imageAvailable = false;
	width = 0;
	height = 0;

	if (url != ""){
		open(url);
	}
}

void VlcCapture::open(std::string &url) {
	vlc = libvlc_new(0, 0);
	vlcm = libvlc_media_new_location(vlc, url.c_str());
	vlcmp = libvlc_media_player_new_from_media (vlcm);
	libvlc_video_set_callbacks(vlcmp, lock_frame, unlock_frame, 0, this);
	libvlc_video_set_format_callbacks(vlcmp, format_callback, 0);
	libvlc_media_release (vlcm);
	start();
}

VlcCapture::~VlcCapture() {
	libvlc_media_player_release (vlcmp);
	libvlc_release (vlc);
	free(buffer);
}

bool VlcCapture::read(cv::Mat& image) {
	pthread_mutex_lock(&imagemutex);
	if (imageAvailable){
		if (image.rows != height || image.cols != width || image.type() != CV_8UC4){
			image = cv::Mat(height,width,CV_8UC4);
		}
		memcpy(image.data, buffer, buffersize);
		imageAvailable = false;
		pthread_mutex_unlock(&imagemutex);
		return true;
	}
	pthread_mutex_unlock(&imagemutex);
	return false;
}

int VlcCapture::start() {
	return libvlc_media_player_play (vlcmp);
}

void VlcCapture::stop() {
	libvlc_media_player_stop (vlcmp);
}

/* user_data is the pointer we pass to `video_set_callbacks()` */
void* lock_frame(void* user_data, void** planes) {
	VlcCapture* parent = (VlcCapture*) user_data;
	pthread_mutex_lock(&parent->imagemutex);

	// /* make '*plane* point to a memory you want the video-data rendered to */
	*planes= parent->buffer;
	parent->imageAvailable = true;
	for(int i=0;i<parent->callbacks.size();i++){
		parent->callbacks[i]->imageCallback();
	}

	return NULL;
}
void unlock_frame(void *user_data, void *picture, void *const *planes) {
	VlcCapture* parent = (VlcCapture*) user_data;
	pthread_mutex_unlock(&parent->imagemutex);

	/* image rendered into (*planes)==parent->buffer; */
}

unsigned format_callback(void**user_data_ptr, char*chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines) {
	VlcCapture* parent =(VlcCapture*)(*user_data_ptr);
	pthread_mutex_lock(&parent->imagemutex);

	/* set the output format to RGBA */
	memcpy(chroma, "RV32", sizeof("RV32") - 1); /* 'RV32' is 'RGBA'
	/* leave dimensions intact, but store them
	* now's the time to resize parent->buffer to hold that much memory
	*/
	parent->width =*width;
	parent->height=*height;
	unsigned int pixel_size = 4; /* 4 is the pixel size for RGBA */
	*pitches=(*width)*pixel_size;
	*lines=*height;

	size_t newBufferSize = parent->width*parent->height*4;
	if (newBufferSize > parent->buffersize){
		parent->buffer = (unsigned char*)realloc(parent->buffer, newBufferSize);
		parent->buffersize = newBufferSize;
	}

	pthread_mutex_unlock(&parent->imagemutex);
	return 1;
}
