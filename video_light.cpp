#include "video_light.h"

#include <iostream>
#include <string>
#include <stdexcept>

// --------------------------------------------------------------------------------------------

VideoLight::VideoLight(int screen_width, int screen_height, const char* video_filename)
{
	this->screen_width = screen_width;
	this->screen_height = screen_height;
    this->video_filename = video_filename;

	int result;

	// initialize libav
	::av_register_all();

	// open video
	if (result = ::avformat_open_input(&fmt_ctx, video_filename, NULL, NULL) < 0) {
		std::cout << "Failed to open input" << std::endl;
        throw std::runtime_error(getFFmpegError(result));
	}

	// find stream info
	if (result = ::avformat_find_stream_info(fmt_ctx, NULL)  < 0) {
		std::cout << "Failed to get stream info" << std::endl;
        throw std::runtime_error(getFFmpegError(result));
	}
	
	// dump debug info
	::av_dump_format(fmt_ctx, 0, video_filename, 0);

	bool use_new_codec_finder = false;

	if(use_new_codec_finder) {
        if (result = this->open_codec_context(&stream_idx, &codec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
            video_stream = fmt_ctx->streams[stream_idx];
        }
	}

	else {
		// find the video stream
		for (unsigned int i = 0; i < fmt_ctx->nb_streams; ++i)
		{
			if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				stream_idx = i;
				break;
			}
		}

		// stream idx ok ?
		if (stream_idx == -1)
		{
			std::cout << "Failed to find video stream" << std::endl;
			throw std::runtime_error("Failed to find video stream");
		}

		video_stream = fmt_ctx->streams[stream_idx];
		codec_ctx = video_stream->codec;

		// find the decoder
		decoder = ::avcodec_find_decoder(codec_ctx->codec_id);
		if (decoder == NULL)
		{
			std::cout << "Failed to find decoder" << std::endl;
			throw std::runtime_error("Failed to find decoder");
		}

		// open the decoder
		if (result = ::avcodec_open2(codec_ctx, decoder, NULL) < 0)
		{
			std::cout << "failed to open codec" << std::endl;
			throw std::runtime_error(getFFmpegError(result));
		}
	}

	// alloc
    av_frame = ::av_frame_alloc();
    gl_frame = ::av_frame_alloc();
	
	// acpicture
    int size = ::avpicture_get_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height);
    uint8_t *internal_buffer = (uint8_t *) ::av_malloc(size * sizeof(uint8_t));
    ::avpicture_fill((AVPicture *)gl_frame, internal_buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height);

	// alloc
    // SLOW: do not use av_init_packet
    //av_init_packet(&packet);
	packet = (AVPacket *) ::av_malloc(sizeof(AVPacket));

	float aspect  = (float)codec_ctx->width / (float)codec_ctx->height;
	//int adj_width = aspect * 600;
	//int adj_height = 600;

    std::cout << "aspect=" << aspect << std::endl;
}

// --------------------------------------------------------------------------------------------

bool VideoLight::readVideoFrame() {	
	do {
		if (::av_read_frame(fmt_ctx, packet) < 0) {
			::av_free_packet(packet);
			return false;
		}
	
		if (packet->stream_index == stream_idx) {
			int frame_finished = 0;
		
			if (::avcodec_decode_video2(codec_ctx, av_frame, &frame_finished, packet) < 0) {
				::av_free_packet(packet);
				return false;
			}
		
			if (frame_finished) {
				if (!conv_ctx) {
					conv_ctx = ::sws_getContext(codec_ctx->width, codec_ctx->height, 
					    codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, 
						AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
				}
			
				::sws_scale(conv_ctx, av_frame->data, av_frame->linesize, 0, 
					codec_ctx->height, gl_frame->data, gl_frame->linesize);

				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, codec_ctx->width, codec_ctx->height, 
				    GL_RGB, GL_UNSIGNED_BYTE, gl_frame->data[0]);
			}
		}
		
		::av_free_packet(packet);

	} while (packet->stream_index != stream_idx);

	return true;
}

// --------------------------------------------------------------------------------------------

int VideoLight::open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;

    ret = ::av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);

    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n", av_get_media_type_string(type), video_filename);
        return ret;

    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];

        /* find decoder for the stream */
        dec = ::avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n", av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }

        /* Allocate a codec context for the decoder */
        *dec_ctx = ::avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n", av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }

        /* Copy codec parameters from input stream to output codec context */
        if ((ret = ::avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n", av_get_media_type_string(type));
            return ret;
        }

        /* Init the decoders, with or without reference counting */
		int refcount = 1;
        ::av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = ::avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n", av_get_media_type_string(type));
            return ret;
        }

        *stream_idx = stream_index;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

VideoLight::~VideoLight()
{
	if (av_frame)  ::av_free(av_frame);
	if (gl_frame)  ::av_free(gl_frame);
	if (packet)    ::av_free(packet);
	if (codec_ctx) ::avcodec_close(codec_ctx);
	if (fmt_ctx)   ::avformat_free_context(fmt_ctx);

	fmt_ctx      = NULL;
	stream_idx   = -1;
	video_stream = NULL;
	codec_ctx    = NULL;
	decoder      = NULL;
	av_frame     = NULL;
	gl_frame     = NULL;
	conv_ctx     = NULL;
}
