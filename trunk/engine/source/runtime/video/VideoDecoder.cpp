
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

#if DC_PLATFORM_WIN32
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
	AVFormatContext* _formatContext = nullptr;
	AVCodecContext* _codecContext = nullptr;
	AVFrame* _frame = nullptr;
	AVFrame* _rgbFrame = nullptr;
	struct SwsContext *_imgConvertCtx = nullptr;

	bool _loop = false;
	bool _closed = true;
	int _videoStream = -1;
	int _targetWidth = -1, _targetHeight = -1;
	byte* _yuv_buffer = nullptr;

	List<VideoFrame*> _decodedFrameCache;
	List<VideoFrame*> _freeFrameCache;

	std::thread _decodeThread;
	std::mutex _mutex;
	std::condition_variable _condition;

	VideoDecoderPrivate()
	{

	}

	~VideoDecoderPrivate()
	{
		this->Close();
	}

	bool OpenFile(const String& path, bool loop, int w, int h)
	{
		DC_PROFILE_FUNCTION;
		bool result = false;
		do 
		{
			if (!g_lib_loaded)
			{
				Debuger::Error("g_lib_loaded");
				break;
			}

			_formatContext = nullptr;
			if (p_avformat_open_input(&_formatContext, path.c_str(), nullptr, nullptr) != 0)
			{
				Debuger::Error("avformat_open_input");
				break;
			}

			if (p_avformat_find_stream_info(_formatContext, nullptr) < 0)
			{
				Debuger::Error("avformat_find_stream_info");
				break;
			}

			_videoStream = -1;
			for (uint32_t i = 0; i < _formatContext->nb_streams; ++i)
			{
				if (_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				{
					_videoStream = i;
					break;
				}
			}
			if (_videoStream < 0)
			{
				Debuger::Error("AVMEDIA_TYPE_VIDEO");
				break;
			}

			const AVCodecParameters* codec_param = _formatContext->streams[_videoStream]->codecpar;
			const AVCodec* codec = p_avcodec_find_decoder(codec_param->codec_id);
			if (codec == nullptr)
			{
				Debuger::Error("avcodec_find_decoder");
				break;
			}

			_codecContext = p_avcodec_alloc_context3(codec);
			if (p_avcodec_parameters_to_context(_codecContext, codec_param) != 0)
			{
				Debuger::Error("avcodec_parameters_to_context");
				break;
			}

			if (p_avcodec_open2(_codecContext, codec, nullptr) < 0)
			{
				Debuger::Error("avcodec_open2");
				break;
			}
			assert(_codecContext->pix_fmt == AV_PIX_FMT_YUV420P);

			_targetWidth = w <= 0 ? _codecContext->width : w;
			_targetHeight = h <= 0 ? _codecContext->height : h;

			_yuv_buffer = Memory::NewArray<byte>(_targetWidth * _targetHeight * 3);
			_frame = p_av_frame_alloc();
			_rgbFrame = p_av_frame_alloc();

#if defined(DC_GRAPHICS_API_DX9)
			AVPixelFormat format = AV_PIX_FMT_BGR24;
#else
			AVPixelFormat format = AV_PIX_FMT_RGB24;
#endif

			if (p_avpicture_fill((AVPicture *)_rgbFrame, _yuv_buffer, format, _targetWidth, _targetHeight) < 0)
			{
				Debuger::Error("avpicture_fill");
				break;
			}
			_imgConvertCtx = p_sws_getContext(_codecContext->width, _codecContext->height, _codecContext->pix_fmt, _targetWidth, _targetHeight, format, SWS_BICUBIC, NULL, NULL, NULL);

			_loop = loop;
			_closed = false;
			_decodeThread = std::thread(&DecodeThread, this);
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

		if (!_closed)
		{
			_mutex.lock();
			_closed = true;
			_mutex.unlock();
			_condition.notify_one();
			_decodeThread.join();
		}

		Memory::DeleteArray(_yuv_buffer);

		for (auto obj : _decodedFrameCache)
		{
			SAFE_DELETE(obj);
		}
		_decodedFrameCache.Clear();
		for (auto obj : _freeFrameCache)
		{
			SAFE_DELETE(obj);
		}
		_freeFrameCache.Clear();

		if (_frame != nullptr)
		{
			p_av_frame_free(&_frame);
			_frame = nullptr;
		}

		if (_codecContext != nullptr)
		{
			p_avcodec_close(_codecContext);
			_codecContext = nullptr;
		}

		if (_formatContext != nullptr)
		{
			p_avformat_close_input(&_formatContext);
			_formatContext = nullptr;
		}

		if (_imgConvertCtx != nullptr)
		{
			p_sws_freeContext(_imgConvertCtx);
			_imgConvertCtx = nullptr;
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
			p->_freeFrameCache.AddLast(VideoFrame::Create(ColorFormat::R8G8B8, iSize(p->_targetWidth, p->_targetHeight)));
		}

		while (true)
		{
			VideoFrame* frame = nullptr;
			{
				std::unique_lock<std::mutex> lock(p->_mutex);
				p->_condition.wait(lock, [p] {
					return p->_freeFrameCache.Size() > 0 || p->_closed;
					});

				if (p->_closed)
				{
					break;
				}

				frame = p->_freeFrameCache.First();
				p->_freeFrameCache.RemoveFirst();
			}

			frame->isLastFrame = false;
			if (!p->DecodeFrame(frame))
			{
				frame->isLastFrame = true;
			}

			{
				std::lock_guard<std::mutex> lock(p->_mutex);
				p->_decodedFrameCache.AddLast(frame);
				p->_condition.notify_one();
			}
		}
	}

	int SeekTime(double time)
	{
		double time_base = r2d(_formatContext->streams[_videoStream]->time_base);
		int64_t timestamp = (int64_t)(time / time_base);
		return p_av_seek_frame(_formatContext, _videoStream, timestamp, AVSEEK_FLAG_BACKWARD);
	}

	bool DecodeFrame(VideoFrame* frame)
	{
		DC_PROFILE_FUNCTION;
		if (!g_lib_loaded || _codecContext == nullptr)
		{
			return false;
		}

	read:
		AVPacket packet = { };
		int ret = p_av_read_frame(_formatContext, &packet);
		if (ret < 0)
		{
			if (ret == AVERROR_EOF)
			{
				if (_loop)
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

		if (packet.stream_index != _videoStream)
		{
			p_av_packet_unref(&packet);
			goto read;
		}

		if (packet.stream_index == _videoStream)
		{
		send:
			p_avcodec_send_packet(_codecContext, &packet);

			ret = p_avcodec_receive_frame(_codecContext, _frame);
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
			int result = p_sws_scale(_imgConvertCtx, (const uint8_t* const*)_frame->data, _frame->linesize, 0, _codecContext->height, _rgbFrame->data, _rgbFrame->linesize);
			if (result < 0)
			{
				Debuger::Error("sws_scale error");
				return false;
			}
			frame->image->Fill(_rgbFrame->data[0], frame->image->GetSize());
			frame->present_time = (float)(packet.pts * r2d(_formatContext->streams[_videoStream]->time_base));
		}

		p_av_packet_unref(&packet);
		return true;
	}

	VideoFrame* GetFrame()
	{
		VideoFrame* frame = nullptr;
		if (_closed)
		{
			return frame;
		}

		std::unique_lock<std::mutex> lock(_mutex);
		_condition.wait(lock, [this] {
			return _decodedFrameCache.Size() > 0 || _closed;
			});

		if (_decodedFrameCache.Size() > 0)
		{
			frame = _decodedFrameCache.First();
			_decodedFrameCache.RemoveFirst();

		}

		return frame;
	}

	void ReleaseFrame(VideoFrame* frame)
	{
		if (_closed)
		{
			return;
		}

		std::unique_lock<std::mutex> lock(_mutex);
		_freeFrameCache.AddLast(frame);
		_condition.notify_one();
	}
};

/********************************************************************/
IMPL_REFECTION_TYPE(VideoDecoder);	
void VideoDecoder::Initialize()
{
#if DC_PLATFORM_WIN32
	LoadFFmpeg();
	if (g_lib_loaded)
	{
		p_av_register_all();
	}
#endif
}
void VideoDecoder::Destroy()
{
#if DC_PLATFORM_WIN32
	FreeFFmpeg();
#endif
}
VideoDecoder::VideoDecoder()
{
#if DC_PLATFORM_WIN32
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
		return _private->_loop;
	}
	return false;
}
int VideoDecoder::GetFrameWidth()const
{
	if (_private)
	{
		return _private->_targetWidth;
	}
	return 0;
}
int VideoDecoder::GetFrameHeight()const
{
	if (_private)
	{
		return _private->_targetHeight;
	}
	return 0;
}

#undef LOAD_DY_LIB
#undef GET_DY_FUNC
#undef FREE_DY_LIB
DC_END_NAMESPACE
