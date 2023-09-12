
#include "VideoDecoder.h"
#include "core/DllHelper.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

DC_BEGIN_NAMESPACE
typedef void(*t_av_register_all)();
static t_av_register_all p_av_register_all;
typedef int(*t_avformat_open_input)(AVFormatContext** ps, const char* url, ff_const59 AVInputFormat* fmt, AVDictionary** options);
static t_avformat_open_input p_avformat_open_input;
typedef void(*t_avformat_close_input)(AVFormatContext** s);
static t_avformat_close_input p_avformat_close_input;
typedef int(*t_avformat_find_stream_info)(AVFormatContext* ic, AVDictionary** options);
static t_avformat_find_stream_info p_avformat_find_stream_info;
typedef AVCodec* (*t_avcodec_find_decoder)(enum AVCodecID id);
static t_avcodec_find_decoder p_avcodec_find_decoder;
typedef AVCodecContext* (*t_avcodec_alloc_context3)(const AVCodec* codec);
static t_avcodec_alloc_context3 p_avcodec_alloc_context3;
typedef int(*t_avcodec_parameters_to_context)(AVCodecContext* codec, const AVCodecParameters* par);
static t_avcodec_parameters_to_context p_avcodec_parameters_to_context;
typedef int(*t_avcodec_open2)(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options);
static t_avcodec_open2 p_avcodec_open2;
typedef int(*t_avcodec_close)(AVCodecContext* avctx);
static t_avcodec_close p_avcodec_close;
typedef AVFrame* (*t_av_frame_alloc)();
static t_av_frame_alloc p_av_frame_alloc;
typedef void(*t_av_frame_free)(AVFrame** frame);
static t_av_frame_free p_av_frame_free;
typedef void(*t_av_frame_unref)(AVFrame* frame);
static t_av_frame_unref p_av_frame_unref;
typedef void(*t_av_freep)(void* ptr);
static t_av_freep p_av_freep;
typedef int64_t(*t_av_gettime_relative)();
static t_av_gettime_relative p_av_gettime_relative;
typedef int(*t_av_read_frame)(AVFormatContext* s, AVPacket* pkt);
static t_av_read_frame p_av_read_frame;
typedef void(*t_av_packet_unref)(AVPacket* pkt);
static t_av_packet_unref p_av_packet_unref;
typedef void(*t_av_init_packet)(AVPacket* pkt);
static t_av_init_packet p_av_init_packet;
typedef int(*t_avcodec_send_packet)(AVCodecContext* avctx, const AVPacket* avpkt);
static t_avcodec_send_packet p_avcodec_send_packet;
typedef int(*t_avcodec_receive_frame)(AVCodecContext* avctx, AVFrame* frame);
static t_avcodec_receive_frame p_avcodec_receive_frame;
typedef int(*t_av_seek_frame)(AVFormatContext* s, int stream_index, int64_t timestamp, int flags);
static t_av_seek_frame p_av_seek_frame;
typedef int(*t_avpicture_fill)(AVPicture *picture, const uint8_t *ptr,enum AVPixelFormat pix_fmt, int width, int height);
static t_avpicture_fill p_avpicture_fill;
typedef int(*t_avpicture_get_size)(enum AVPixelFormat pix_fmt, int width, int height);
static t_avpicture_get_size p_avpicture_get_size;
typedef struct SwsContext*(*t_sws_getContext)(int srcW, int srcH, enum AVPixelFormat srcFormat,int dstW, int dstH, enum AVPixelFormat dstFormat,int flags, SwsFilter *srcFilter,SwsFilter *dstFilter, const double *param);
static t_sws_getContext p_sws_getContext;
typedef int(*t_sws_scale)(struct SwsContext *c, const uint8_t *const srcSlice[],const int srcStride[], int srcSliceY, int srcSliceH,uint8_t *const dst[], const int dstStride[]);
static t_sws_scale p_sws_scale;
typedef void(*t_sws_freeContext)(struct SwsContext *swsContext);
static t_sws_freeContext p_sws_freeContext;

static bool g_lib_loaded = false;

#if DC_PLATFORM_MAC || DC_PLATFORM_WIN32
static LIB_HANDLE g_libswresample;
static LIB_HANDLE g_libavutil;
static LIB_HANDLE g_libavcodec;
static LIB_HANDLE g_libavformat;
static LIB_HANDLE g_libswscale;

static void LoadFFmpeg()
{
	String lib_dir = ".";

#if DC_PLATFORM_WIN32
	String lib_path = lib_dir + "/swresample-3.dll";
#endif
#if DC_PLATFORM_MAC
	String lib_path = lib_dir + "/libswresample.3.dylib";
#endif
	LOAD_DY_LIB(g_libswresample, lib_path.c_str());

#if DC_PLATFORM_WIN32
	lib_path = lib_dir + "/avutil-56.dll";
#endif
#if DC_PLATFORM_MAC
	lib_path = lib_dir + "/libavutil.56.dylib";
#endif
	LOAD_DY_LIB(g_libavutil, lib_path.c_str());

#if DC_PLATFORM_WIN32
	lib_path = lib_dir + "/avcodec-58.dll";
#endif
#if DC_PLATFORM_MAC
	lib_path = lib_dir + "/libavcodec.58.dylib";
#endif
	LOAD_DY_LIB(g_libavcodec, lib_path.c_str());

#if DC_PLATFORM_WIN32
	lib_path = lib_dir + "/avformat-58.dll";
#endif
#if DC_PLATFORM_MAC
	lib_path = lib_dir + "/libavformat.58.dylib";
#endif
	LOAD_DY_LIB(g_libavformat, lib_path.c_str());

#if DC_PLATFORM_WIN32
	lib_path = lib_dir + "/swscale-5.dll";
#endif
#if DC_PLATFORM_MAC
	lib_path = lib_dir + "/swscale.5.dylib";
#endif
	LOAD_DY_LIB(g_libswscale, lib_path.c_str());

	p_av_register_all		= (t_av_register_all)GET_DY_FUNC(g_libavformat, "av_register_all");
	p_avformat_open_input	= (t_avformat_open_input)GET_DY_FUNC(g_libavformat, "avformat_open_input");
	p_avformat_close_input	= (t_avformat_close_input)GET_DY_FUNC(g_libavformat, "avformat_close_input");
	p_avformat_find_stream_info = (t_avformat_find_stream_info)GET_DY_FUNC(g_libavformat, "avformat_find_stream_info");
	p_av_read_frame			= (t_av_read_frame)GET_DY_FUNC(g_libavformat, "av_read_frame");
	p_av_seek_frame			= (t_av_seek_frame)GET_DY_FUNC(g_libavformat, "av_seek_frame");

	p_avcodec_find_decoder	= (t_avcodec_find_decoder)GET_DY_FUNC(g_libavcodec, "avcodec_find_decoder");
	p_avcodec_alloc_context3 = (t_avcodec_alloc_context3)GET_DY_FUNC(g_libavcodec, "avcodec_alloc_context3");
	p_avcodec_parameters_to_context = (t_avcodec_parameters_to_context)GET_DY_FUNC(g_libavcodec, "avcodec_parameters_to_context");
	p_avcodec_open2			= (t_avcodec_open2)GET_DY_FUNC(g_libavcodec, "avcodec_open2");
	p_avcodec_close			= (t_avcodec_close)GET_DY_FUNC(g_libavcodec, "avcodec_close");
	p_av_packet_unref		= (t_av_packet_unref)GET_DY_FUNC(g_libavcodec, "av_packet_unref");
	p_av_init_packet		= (t_av_init_packet)GET_DY_FUNC(g_libavcodec, "av_init_packet");
	p_avcodec_send_packet	= (t_avcodec_send_packet)GET_DY_FUNC(g_libavcodec, "avcodec_send_packet");
	p_avcodec_receive_frame = (t_avcodec_receive_frame)GET_DY_FUNC(g_libavcodec, "avcodec_receive_frame");
	p_avpicture_fill		= (t_avpicture_fill)GET_DY_FUNC(g_libavcodec, "avpicture_fill");
	p_avpicture_get_size	= (t_avpicture_get_size)GET_DY_FUNC(g_libavcodec, "avpicture_get_size");

	p_av_frame_alloc		= (t_av_frame_alloc)GET_DY_FUNC(g_libavutil, "av_frame_alloc");
	p_av_frame_free			= (t_av_frame_free)GET_DY_FUNC(g_libavutil, "av_frame_free");
	p_av_frame_unref		= (t_av_frame_unref)GET_DY_FUNC(g_libavutil, "av_frame_unref");
	p_av_freep				= (t_av_freep)GET_DY_FUNC(g_libavutil, "av_freep");
	p_av_gettime_relative	= (t_av_gettime_relative)GET_DY_FUNC(g_libavutil, "av_gettime_relative");

	p_sws_getContext		= (t_sws_getContext)GET_DY_FUNC(g_libswscale, "sws_getContext");
	p_sws_scale				= (t_sws_scale)GET_DY_FUNC(g_libswscale, "sws_scale");
	p_sws_freeContext		= (t_sws_freeContext)GET_DY_FUNC(g_libswscale, "sws_freeContext");

	g_lib_loaded = true;
}
static void FreeFFmpeg()
{
	if (g_lib_loaded)
	{
		FREE_DY_LIB(g_libavformat);
		FREE_DY_LIB(g_libavcodec);
		FREE_DY_LIB(g_libavutil);
		FREE_DY_LIB(g_libswresample);
		FREE_DY_LIB(g_libswscale);
		g_lib_loaded = false;
	}
}
#endif
/********************************************************************/

class VideoDecoderPrivate : public object
{
	DEFAULT_CREATE(VideoDecoderPrivate);
	FRIEND_CONSTRUCT_DESTRUCT(VideoDecoderPrivate);
public:
	AVFormatContext* m_format_context = nullptr;
	AVCodecContext* m_codec_context = nullptr;
	AVFrame* m_frame = nullptr;
	AVFrame* m_rgb_frame = nullptr;
	struct SwsContext *m_img_convert_ctx = nullptr;

	bool m_loop = false;
	bool m_closed = true;
	int m_video_stream = -1;
	int m_target_width = -1, m_target_height = -1;
	byte* m_yuv_buffer = nullptr;

	List<VideoFrame*> m_decoded_frame_cache;
	List<VideoFrame*> m_free_frame_cache;

	std::thread m_decode_thread;
	std::mutex _mutex;
	std::condition_variable m_condition;

	VideoDecoderPrivate()
	{

	}

	~VideoDecoderPrivate()
	{
		this->Close();
	}

	bool OpenFile(const String& path, bool loop, int w, int h)
	{
		bool result = false;
		do 
		{
			if (!g_lib_loaded)
			{
				Debuger::Error("g_lib_loaded");
				break;
			}

			m_format_context = nullptr;
			if (p_avformat_open_input(&m_format_context, path.c_str(), nullptr, nullptr) != 0)
			{
				Debuger::Error("avformat_open_input");
				break;
			}

			if (p_avformat_find_stream_info(m_format_context, nullptr) < 0)
			{
				Debuger::Error("avformat_find_stream_info");
				break;
			}

			m_video_stream = -1;
			for (uint32_t i = 0; i < m_format_context->nb_streams; ++i)
			{
				if (m_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					m_video_stream = i;
					break;
				}
			}
			if (m_video_stream < 0)
			{
				Debuger::Error("AVMEDIA_TYPE_VIDEO");
				break;
			}

			const AVCodecParameters* codec_param = m_format_context->streams[m_video_stream]->codecpar;
			const AVCodec* codec = p_avcodec_find_decoder(codec_param->codec_id);
			if (codec == nullptr)
			{
				Debuger::Error("avcodec_find_decoder");
				break;
			}

			m_codec_context = p_avcodec_alloc_context3(codec);
			if (p_avcodec_parameters_to_context(m_codec_context, codec_param) != 0)
			{
				Debuger::Error("avcodec_parameters_to_context");
				break;
			}

			if (p_avcodec_open2(m_codec_context, codec, nullptr) < 0)
			{
				Debuger::Error("avcodec_open2");
				break;
			}
			assert(m_codec_context->pix_fmt == AV_PIX_FMT_YUV420P);

			m_target_width = w <= 0 ? m_codec_context->width : w;
			m_target_height = h <= 0 ? m_codec_context->height : h;

			m_yuv_buffer = NewArray<byte>(m_target_width * m_target_height * 3);
			m_frame = p_av_frame_alloc();
			m_rgb_frame = p_av_frame_alloc();

#if defined(DC_GRAPHICS_API_DX9)
			AVPixelFormat format = AV_PIX_FMT_BGR24;
#else
			AVPixelFormat format = AV_PIX_FMT_RGB24;
#endif

			if (p_avpicture_fill((AVPicture *)m_rgb_frame, m_yuv_buffer, format, m_target_width, m_target_height) < 0)
			{
				Debuger::Error("avpicture_fill");
				break;
			}
			m_img_convert_ctx = p_sws_getContext(m_codec_context->width, m_codec_context->height, m_codec_context->pix_fmt, m_target_width, m_target_height, format, SWS_BICUBIC, NULL, NULL, NULL);

			m_loop = loop;
			m_closed = false;
			m_decode_thread = std::thread(&DecodeThread, this);
			result = true;
		} while (false);

		if (!result)
		{
			this->Close();
		}
		return result;
	}

	void Close()
	{
		if (!g_lib_loaded)
		{
			return;
		}

		if (!m_closed)
		{
			_mutex.lock();
			m_closed = true;
			_mutex.unlock();
			m_condition.notify_one();
			m_decode_thread.join();
		}

		DeleteArray(m_yuv_buffer);

		for (auto obj : m_decoded_frame_cache)
		{
			SAFE_DELETE(obj);
		}
		m_decoded_frame_cache.Clear();
		for (auto obj : m_free_frame_cache)
		{
			SAFE_DELETE(obj);
		}
		m_free_frame_cache.Clear();

		if (m_frame != nullptr)
		{
			p_av_frame_free(&m_frame);
			m_frame = nullptr;
		}

		if (m_codec_context != nullptr)
		{
			p_avcodec_close(m_codec_context);
			m_codec_context = nullptr;
		}

		if (m_format_context != nullptr)
		{
			p_avformat_close_input(&m_format_context);
			m_format_context = nullptr;
		}

		if (m_img_convert_ctx != nullptr)
		{
			p_sws_freeContext(m_img_convert_ctx);
			m_img_convert_ctx = nullptr;
		}
	}

	static double r2d(AVRational r)
	{
		return r.num == 0 || r.den == 0 ? 0.0 : (double)r.num / (double)r.den;
	}

	static void DecodeThread(VideoDecoderPrivate* p)
	{
		const int MAX_CACHE_FRAMES = 10;
		for (int i = 0; i < MAX_CACHE_FRAMES; ++i)
		{
			p->m_free_frame_cache.AddLast(VideoFrame::Create(ColorFormat::R8G8B8, iSize(p->m_target_width, p->m_target_height)));
		}

		while (true)
		{
			VideoFrame* frame = nullptr;
			{
				std::unique_lock<std::mutex> lock(p->_mutex);
				p->m_condition.wait(lock, [p] {
					return p->m_free_frame_cache.Size() > 0 || p->m_closed;
					});

				if (p->m_closed)
				{
					break;
				}

				frame = p->m_free_frame_cache.First();
				p->m_free_frame_cache.RemoveFirst();
			}

			frame->is_last_frame = false;
			if (!p->DecodeFrame(frame))
			{
				frame->is_last_frame = true;
			}

			{
				std::lock_guard<std::mutex> lock(p->_mutex);
				p->m_decoded_frame_cache.AddLast(frame);
				p->m_condition.notify_one();
			}
		}
	}

	int SeekTime(double time)
	{
		double time_base = r2d(m_format_context->streams[m_video_stream]->time_base);
		int64_t timestamp = (int64_t)(time / time_base);
		return p_av_seek_frame(m_format_context, m_video_stream, timestamp, AVSEEK_FLAG_BACKWARD);
	}

	bool DecodeFrame(VideoFrame* frame)
	{
		if (!g_lib_loaded || m_codec_context == nullptr)
		{
			return false;
		}

	read:
		AVPacket packet = { };
		int ret = p_av_read_frame(m_format_context, &packet);
		if (ret < 0)
		{
			if (ret == AVERROR_EOF)
			{
				if (m_loop)
				{
					ret = this->SeekTime(0);
					if (ret < 0)
						return false;
					else
						goto read;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (packet.stream_index != m_video_stream)
		{
			p_av_packet_unref(&packet);
			goto read;
		}

		if (packet.stream_index == m_video_stream)
		{
		send:
			p_avcodec_send_packet(m_codec_context, &packet);

			ret = p_avcodec_receive_frame(m_codec_context, m_frame);
			if (ret < 0)
			{
				if (ret == AVERROR(EAGAIN))
				{
					goto send;
				}
				else
				{
					return false;
				}
			}

			//转换
			int result = p_sws_scale(m_img_convert_ctx, (const uint8_t* const*)m_frame->data, m_frame->linesize, 0, m_codec_context->height, m_rgb_frame->data, m_rgb_frame->linesize);
			if (result < 0)
			{
				Debuger::Error("sws_scale error");
				return false;
			}
			frame->image->Fill(m_rgb_frame->data[0], frame->image->GetSize());
			frame->present_time = (float)(packet.pts * r2d(m_format_context->streams[m_video_stream]->time_base));
		}

		p_av_packet_unref(&packet);
		return true;
	}

	VideoFrame* GetFrame()
	{
		VideoFrame* frame = nullptr;
		if (m_closed)
		{
			return frame;
		}

		std::unique_lock<std::mutex> lock(_mutex);
		m_condition.wait(lock, [this] {
			return m_decoded_frame_cache.Size() > 0 || m_closed;
			});

		if (m_decoded_frame_cache.Size() > 0)
		{
			frame = m_decoded_frame_cache.First();
			m_decoded_frame_cache.RemoveFirst();

		}

		return frame;
	}

	void ReleaseFrame(VideoFrame* frame)
	{
		if (m_closed)
		{
			return;
		}

		std::unique_lock<std::mutex> lock(_mutex);
		m_free_frame_cache.AddLast(frame);
		m_condition.notify_one();
	}
};

/********************************************************************/
IMPL_REFECTION_TYPE(VideoDecoder);	
void VideoDecoder::Initialize()
{
#if DC_PLATFORM_MAC || DC_PLATFORM_WIN32
	LoadFFmpeg();
	if (g_lib_loaded)
	{
		p_av_register_all();
	}
#endif
}
void VideoDecoder::Destroy()
{
#if DC_PLATFORM_MAC || DC_PLATFORM_WIN32
	FreeFFmpeg();
#endif
}
VideoDecoder::VideoDecoder()
{
#if DC_PLATFORM_MAC || DC_PLATFORM_WIN32
	_private = VideoDecoderPrivate::Create();
#endif
}
VideoDecoder::~VideoDecoder()
{
	SAFE_DELETE(_private);
}
bool VideoDecoder::OpenFile(const String& path, bool loop, int w, int h)
{
	if (_private)
	{
		return _private->OpenFile(path, loop, w, h);
	}
	return false;
}
void VideoDecoder::Close()
{
	if (_private)
	{
		_private->Close();
	}
}
VideoFrame* VideoDecoder::GetFrame()
{
	if (_private)
	{
		return _private->GetFrame();
	}
	return nullptr;
}
void VideoDecoder::ReleaseFrame(VideoFrame* frame)
{
	if (_private)
	{
		_private->ReleaseFrame(frame);
	}
}
bool VideoDecoder::IsLoop()const
{
	if (_private)
	{
		return _private->m_loop;
	}
	return false;
}
int VideoDecoder::GetFrameWidth()const
{
	if (_private)
	{
		return _private->m_target_width;
	}
	return 0;
}
int VideoDecoder::GetFrameHeight()const
{
	if (_private)
	{
		return _private->m_target_height;
	}
	return 0;
}

#undef LOAD_DY_LIB
#undef GET_DY_FUNC
#undef FREE_DY_LIB
DC_END_NAMESPACE
