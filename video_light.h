#pragma once

#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern "C" {
	//#include <libavutil/avutil.h>
	//#include <libavutil/pixdesc.h>

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/hwcontext.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

/*---------------------------------------------------------------------------*/

namespace {
    static inline std::string getFFmpegError(int errorCode)
    {
        char errorString[1024];
        ::av_make_error_string(errorString, 1024, errorCode);
        std::cout << "FFMPEG error: " << errorCode << " " << errorString << std::endl;

        return std::string(errorString);
    }
};

/*---------------------------------------------------------------------------*/

class VideoLight
{
	public:
		VideoLight(int screen_width, int screen_height, const char* video_filenam);
		virtual ~VideoLight();

	public:
       	int screen_width;
       	int screen_height;
		const char* video_filename;

	public:
		bool readVideoFrame();
		int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);

	public:
		AVFormatContext *fmt_ctx = NULL;
		int             stream_idx = -1;
		AVStream        *video_stream = NULL;
		AVCodecContext  *codec_ctx = NULL;
		AVCodec         *decoder = NULL;
		AVPacket        *packet;
		AVFrame         *av_frame = NULL;
		AVFrame         *gl_frame = NULL;

		struct SwsContext *conv_ctx = NULL;

};