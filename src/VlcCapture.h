/*
 * VlcCapture.h
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#ifndef VLCCAPTURE_H_
#define VLCCAPTURE_H_


//https://wiki.videolan.org/Stream_to_memory_%28smem%29_tutorial/

#include <stdio.h>
#include <stdint.h>
#include <string>

#include <vlc/vlc.h>

#include <pthread.h>
#include <opencv2/opencv.hpp>


/**
 * lock and set the pointer to image buffer
 */
void pf_video_prerender_callback ( void* p_video_data, uint8_t** pp_pixel_buffer, size_t size );

/**
 * image buffer full, unlock at end
 */
void pf_video_postrender_callback ( void* p_video_data, uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch, size_t size, libvlc_time_t pts );

void pf_audio_prerender_callback ( void* p_audio_data, uint8_t** pp_pcm_buffer, size_t size );
void pf_audio_postrender_callback ( void* p_audio_data, uint8_t* p_pcm_buffer, unsigned int channels, unsigned int rate, unsigned int nb_samples, unsigned int bits_per_sample, size_t size, libvlc_time_t pts );



class VlcCapture {
public:
	VlcCapture(std::string url = "", int input_buffer_ms = 10);
	virtual ~VlcCapture();


	void open(std::string &url);

	//bool grab();

	//bool retrieve();

	bool read(cv::Mat &image);

	int start();

	void stop();


	pthread_mutex_t imagemutex;

	static pthread_mutex_t callbackmutex;

	int width,height;

	//cv::Mat imagebuf;
	uint8_t* buffer;
	unsigned int buffersize;

	bool imageAvailable;

	std::string _url;


	void prerender_callback(void* p_video_data, uint8_t** pp_pixel_buffer, size_t size);
	void postrender_callback(void* p_video_data, uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch, size_t size, libvlc_time_t pts );


private:



	int buffer_ms;

    //options
    char smem_options[256];

    //callback pointer addess string
    char str_smem_vid_prerender[100], str_smem_vid_postrender[100];
    char str_smem_aud_prerender[100], str_smem_aud_postrender[100];
    char str_smem_data[100],str_netbuf[100];

    libvlc_instance_t *vlc;
    libvlc_media_player_t *vlcmp;
    libvlc_media_t *vlcm;

};

#endif /* VLCTOMEMORY_H_ */
