/*
 * VlcCapture.cpp
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#include "VlcCapture.h"
#include <unistd.h>

VlcCapture::VlcCapture(std::string url, int input_buffer_ms):buffer_ms(input_buffer_ms) {

	pthread_mutex_init(&imagemutex,NULL);// = PTHREAD_MUTEX_INITIALIZER;

	buffersize = 640*480;
	buffer = (uint8_t*)malloc(buffersize);
	imageAvailable = false;
	width = 0;
	height = 0;

	//https://gist.github.com/TimSC/4121862
	sprintf(str_smem_vid_prerender, "--sout-smem-video-prerender-callback=%lld", (long long int)pf_video_prerender_callback);
	sprintf(str_smem_vid_postrender, "--sout-smem-video-postrender-callback=%lld", (long long int)pf_video_postrender_callback);

	sprintf(str_smem_aud_prerender, "--sout-smem-audio-prerender-callback=%lld", (long long int)pf_audio_prerender_callback);
	sprintf(str_smem_aud_postrender, "--sout-smem-audio-postrender-callback=%lld", (long long int)pf_audio_postrender_callback);

	sprintf(str_smem_data, "--sout-smem-video-data=%lld", (long long int)this);

	//set vlc transcode options: transcode whatever is open to rgb24 in memory (video only)
	sprintf(smem_options,"#transcode{vcodec=RV24,acodec=none}:smem{video-prerender-callback=%lld, video-postrender-callback=%lld, video-data=%lld}",(long long int)pf_video_prerender_callback,(long long int)pf_video_postrender_callback,(long long int)this);
  //	sprintf(smem_options,"#duplicate{dst=display,dst=transcode{vcodec=RV24,acodec=none}:smem{video-prerender-callback=%lld, video-postrender-callback=%lld, video-data=%lld}}",(long long int)pf_video_prerender_callback,(long long int)pf_video_postrender_callback,(long long int)this);

	sprintf(str_netbuf,"--network-caching=%i",buffer_ms);

	//printf("%s\n",str_netbuf);

	//local output
	//sprintf(smem_options,"#transcode{vcodec=RV24,acodec=none}:duplicate{dst=smem,dst=display}");

	if (url != ""){
		open(url);
	}
}

void VlcCapture::open(std::string &url) {
	const char * const vlc_args[] = {
		"-I","dummy",
//		"-vvv",
		"--ignore-config",
		str_netbuf,
//		"--sout-smem-time-sync",
//		str_smem_vid_prerender,
//		str_smem_vid_postrender,
//		str_smem_aud_prerender,
//		str_smem_aud_postrender,
//		str_smem_data,
		"--sout",
		smem_options
	};
	vlc = libvlc_new (sizeof (vlc_args) / sizeof (vlc_args[0]), vlc_args);
	vlcm = libvlc_media_new_location(vlc, url.c_str());
	vlcmp = libvlc_media_player_new_from_media (vlcm);
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
	//imagebuf.copyTo(image);

	if (imageAvailable){
  	if (image.rows != height || image.cols != width || image.type() != CV_8UC3){
      image = cv::Mat(height,width,CV_8UC3);
    }
		memcpy(image.data,buffer,buffersize);
		imageAvailable = false;
    //imshow( "image", image );
		pthread_mutex_unlock(&imagemutex);
      
		return true;
	}/*else{
		usleep(10000);
	}*/
	pthread_mutex_unlock(&imagemutex);
	return false;
}

int VlcCapture::start() {
	return libvlc_media_player_play (vlcmp);
}

void VlcCapture::stop() {
	libvlc_media_player_stop (vlcmp);
}

void pf_video_prerender_callback(void* p_video_data,
		uint8_t** pp_pixel_buffer, size_t size) {

	VlcCapture* parent = (VlcCapture*)p_video_data;

	pthread_mutex_lock(&parent->imagemutex);

	if (size > parent->buffersize){
		parent->buffer = (uint8_t*)realloc(parent->buffer,size);
		parent->buffersize = size;
	}

	*pp_pixel_buffer = parent->buffer;

}

void pf_video_postrender_callback(void* p_video_data,
		uint8_t* p_pixel_buffer, int width, int height, int pixel_pitch,
		size_t size, libvlc_time_t pts) {
	VlcCapture* parent = (VlcCapture*)p_video_data;

	parent->width = width;
	parent->height = height;
	parent->imageAvailable = true;
        for(int i=0;i<parent->callbacks.size();i++){
            parent->callbacks[i]->imageCallback();
        }
	pthread_mutex_unlock(&parent->imagemutex);
}

void pf_audio_prerender_callback(void* p_audio_data,
		uint8_t** pp_pcm_buffer, size_t size) {

	printf("audio size %li\n",size);
	*pp_pcm_buffer = (uint8_t*)malloc(size);

}

void pf_audio_postrender_callback(void* p_audio_data,
		uint8_t* p_pcm_buffer, unsigned int channels, unsigned int rate,
		unsigned int nb_samples, unsigned int bits_per_sample, size_t size,
		libvlc_time_t pts) {
	printf("audio post \n");
	delete p_pcm_buffer;
}
