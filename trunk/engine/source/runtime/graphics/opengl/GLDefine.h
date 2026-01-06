 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/9
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"

#if defined(DC_PLATFORM_ANDROID)
#	define GL_GLEXT_PROTOTYPES
#	include <EGL/egl.h>
#	include <EGL/eglext.h>
#	include <GLES3/gl32.h>
#elif defined(DC_PLATFORM_WIN32)
#	define GL_GLEXT_PROTOTYPES
#	define GLFW_EXPOSE_NATIVE_WIN32
#	if defined(DC_GRAPHICS_API_OPENGLES3)
#		define GLFW_INCLUDE_ES32
#		define GLFW_INCLUDE_GLEXT
#		include <glfw3/glfw3.h>
#		include <glfw3/glfw3native.h>
#		include <EGL/egl.h>
#		include <EGL/eglext.h>
#	else
#		include "external/glad/glad.h"
#	endif
#elif defined(DC_PLATFORM_LINUX)
#	include "external/glad/glad.h"
#elif defined(DC_PLATFORM_WASM)
#	define GL_GLEXT_PROTOTYPES
#	define GLFW_INCLUDE_ES3						//最高支持gles3.0
#	define GLFW_INCLUDE_GLEXT
#	include <GLFW/glfw3.h>						//使用emscripten自带的
#elif defined(DC_PLATFORM_MAC)
#	include "external/glad/glad.h"				//mac最高支持opengl4.1
#elif defined(DC_PLATFORM_IOS)
#	define GL_GLEXT_PROTOTYPES
#	define GLFW_INCLUDE_ES3						//iOS最高支持gles3.0
#	define GLFW_INCLUDE_GLEXT
#endif

#if !defined(glClearDepth)
#	define glClearDepth(v)						glClearDepthf((float)v)
#endif
#if !defined(GL_TEXTURE_WRAP_R)
#	define GL_TEXTURE_WRAP_R					0x8072
#endif
#if !defined(GL_ETC1_RGB8_OES)
#	define GL_ETC1_RGB8_OES						0x8D64
#endif

#if !defined(GL_COMPRESSED_RGBA_ASTC_4x4)
#	define GL_COMPRESSED_RGBA_ASTC_4x4			GL_COMPRESSED_RGBA_ASTC_4x4_KHR
#	define GL_COMPRESSED_RGBA_ASTC_5x4			GL_COMPRESSED_RGBA_ASTC_5x4_KHR
#	define GL_COMPRESSED_RGBA_ASTC_5x5      	GL_COMPRESSED_RGBA_ASTC_5x5_KHR
#	define GL_COMPRESSED_RGBA_ASTC_6x5      	GL_COMPRESSED_RGBA_ASTC_6x5_KHR
#	define GL_COMPRESSED_RGBA_ASTC_6x6      	GL_COMPRESSED_RGBA_ASTC_6x6_KHR
#	define GL_COMPRESSED_RGBA_ASTC_8x5      	GL_COMPRESSED_RGBA_ASTC_8x5_KHR
#	define GL_COMPRESSED_RGBA_ASTC_8x6      	GL_COMPRESSED_RGBA_ASTC_8x6_KHR
#	define GL_COMPRESSED_RGBA_ASTC_8x8      	GL_COMPRESSED_RGBA_ASTC_8x8_KHR
#	define GL_COMPRESSED_RGBA_ASTC_10x5     	GL_COMPRESSED_RGBA_ASTC_10x5_KHR
#	define GL_COMPRESSED_RGBA_ASTC_10x6     	GL_COMPRESSED_RGBA_ASTC_10x6_KHR
#	define GL_COMPRESSED_RGBA_ASTC_10x8     	GL_COMPRESSED_RGBA_ASTC_10x8_KHR
#	define GL_COMPRESSED_RGBA_ASTC_10x10    	GL_COMPRESSED_RGBA_ASTC_10x10_KHR
#	define GL_COMPRESSED_RGBA_ASTC_12x10    	GL_COMPRESSED_RGBA_ASTC_12x10_KHR
#	define GL_COMPRESSED_RGBA_ASTC_12x12    	GL_COMPRESSED_RGBA_ASTC_12x12_KHR
#endif
#ifndef GL_IMG_texture_compression_pvrtc
#	define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG	0x8C00
#	define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG	0x8C01
#	define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG	0x8C02
#	define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG	0x8C03
#endif

//支持opengles3.0
#ifndef GL_GEOMETRY_SHADER
#	define GL_GEOMETRY_SHADER					GL_GEOMETRY_SHADER_OES
#endif
#ifndef GL_TESS_CONTROL_SHADER
#	define GL_TESS_CONTROL_SHADER				GL_TESS_CONTROL_SHADER_OES
#endif
#ifndef GL_TESS_EVALUATION_SHADER
#	define GL_TESS_EVALUATION_SHADER			GL_TESS_EVALUATION_SHADER_OES
#endif
#ifndef GL_COMPUTE_SHADER
#	define GL_COMPUTE_SHADER					0x91B9
#endif
#ifndef GL_CLAMP_TO_BORDER
#	define GL_CLAMP_TO_BORDER					GL_CLAMP_TO_BORDER_OES
#endif
#ifndef GL_TEXTURE_BORDER_COLOR
#	define GL_TEXTURE_BORDER_COLOR				GL_TEXTURE_BORDER_COLOR_OES
#endif

DC_BEGIN_NAMESPACE

#if DC_DEBUG
#	define GL_ERROR(x) do { {x;} GLenum err = glGetError(); AssertEx((err == 0), "0x%04x", (uint)err); } while(0)
#	define GL_CHECK_ERROR() do { GLenum err = glGetError(); AssertEx((err == 0), "0x%04x", (uint)err); } while(0)
#else
#	define GL_ERROR(x) UNUSED(x)
#	define GL_CHECK_ERROR(x) UNUSED(x)
#endif
#define GL_DEBUG_ERROR(code, msg) AssertEx(((code) == 0), msg)
#define GL_BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))

//后台缓冲区
inline uint			GLBackBufferCount = 2;
inline ColorFormat	GLRenderTargetFormat = ColorFormat::R8G8B8A8;
inline ColorFormat	GLDepthStencilFormat = ColorFormat::DEPTH24_STENCIL8;

enum class GLShaderModel
{
	UNKNOWN = 0,
#if defined(DC_GRAPHICS_API_OPENGLES3)
	SM_10,	//固定管线
	SM_11,	//固定管线
	SM_20,	//Programable Vertex and Fragment Shaders 
	SM_30,	//Texture arrays, Multiple Render Targets 
	SM_31,	//Compute Shaders 
	SM_32,	//Tessellation and Geometry Shaders
#else
	// Desktop
	SM_10,	//1.0~1.5:固定管线
	SM_11,
	SM_12,
	SM_13,
	SM_14,
	SM_15,
	SM_20,	//2.0和2.1:固定管线和可编程管线并存
	SM_21,
	SM_30,	//3.0开始是可编程管线
	SM_31,
	SM_32,
	SM_33,
	SM_40,
	SM_41,
	SM_42,
	SM_43,
	SM_44,
	SM_45,
	SM_46,
	SM_50,
#endif
};
DECLARE_ENUM_OPERATORS(GLShaderModel);

//shader类型
inline const GLenum GL_ShaderType[int(ShaderType::Max)] = 
{ 
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,

	GL_FRAGMENT_SHADER,
	(GLenum)-1
};

inline GLenum GL_GetCullMode(CullMode value)
{
	switch (value)
	{
	case CullMode::Off:return GL_NONE;
	case CullMode::Front:return GL_FRONT;
	case CullMode::Back:return GL_BACK;
	default:AssertEx(false, "%d", value); return GL_NONE;
	}
}
inline GLenum GL_GetFillMode(FillMode value)
{
#if defined(DC_GRAPHICS_API_OPENGL)
	switch (value)
	{
	case FillMode::Point:return GL_POINT;
	case FillMode::Wire:return GL_LINE;
	case FillMode::Solid:return GL_FILL;
	default:AssertEx(false, "%d", value); return GL_FILL;
	}
#endif
	return 0;
}
inline GLenum GL_GetShadeMode(ShadeMode value)
{
#if defined(DC_GRAPHICS_API_OPENGL)
	switch (value)
	{
	case ShadeMode::Flat:return GL_FLAT;
	case ShadeMode::Gouraud:return GL_SMOOTH;
	default:AssertEx(0, "%d", value); return GL_SMOOTH;
	}
#endif
	return 0;
}
constexpr GLenum GL_GetIndexType(IndexType value)
{
	return value == IndexType::B16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
}
constexpr uint GL_GetIndexSize(IndexType value)
{
	return value == IndexType::B16 ? 2 : 4;
}
inline GLenum GL_GetTextureType(TextureType value, bool msaa)
{
	msaa = false;//TODO:暂时未开启
	switch (value)
	{
	case TextureType::D2:return (msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
	case TextureType::D3: return GL_TEXTURE_3D;
	case TextureType::Cube:return GL_TEXTURE_CUBE_MAP;
	case TextureType::D2Array:return (msaa ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY);
	default: AssertEx(false, "%d", value); return GL_TEXTURE_2D;
	}
}
//1.只有GL_NEAREST和GL_LINEAR不需要生成mipmap，其他都要，如果用了而没有生成，贴图表现会异常
//GL_NEAREST				使用最邻近过滤
//GL_LINEAR					使用线性过滤
//GL_NEAREST_MIPMAP_NEAREST	选择最邻近的mip层，并使用最邻近过滤
//GL_NEAREST_MIPMAP_LINEAR	在mip层之间使用线性插值和最邻近过滤
//GL_LINEAR_MIPMAP_NEAREST	选择最邻近的mip层，使用线性过滤
//GL_LINEAR_MIPMAP_LINEAR	在mip层之间使用线性插值和使用线性过滤，又称三线性mipmap
inline GLenum GL_GetMinTextureFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::Nearest: return GL_NEAREST;
	case TextureFilter::Bilinear: return GL_LINEAR;
	case TextureFilter::Trilinear: return GL_LINEAR_MIPMAP_LINEAR;
	default:AssertEx(false, "%d", filter); return GL_LINEAR;
	}
}
//放大过滤器只有GL_NEAREST、GL_LINEAR，没有mip
inline GLenum GL_GetMagTextureFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::Nearest: return GL_NEAREST;
	case TextureFilter::Bilinear: return GL_LINEAR;
	default: return GL_LINEAR;
	}
}
inline GLenum GL_GetTextureAddress(TextureAddress value)
{
	switch (value)
	{
	case TextureAddress::Wrap:return GL_REPEAT;
	case TextureAddress::Mirror:return GL_MIRRORED_REPEAT;
	case TextureAddress::Clamp:return GL_CLAMP_TO_EDGE;
	case TextureAddress::Border:return GL_CLAMP_TO_BORDER;
	default:AssertEx(false, "%d", value); return GL_REPEAT;
	}
}
inline GLenum GL_GetAlphaBlend(AlphaBlend value)
{
	switch (value)
	{
	case AlphaBlend::Zero:return GL_ZERO;
	case AlphaBlend::One:return GL_ONE;
	case AlphaBlend::SrcColor:return GL_SRC_COLOR;
	case AlphaBlend::OneMinusSrcColor:return GL_ONE_MINUS_SRC_COLOR;
	case AlphaBlend::SrcAlpha:return GL_SRC_ALPHA;
	case AlphaBlend::OneMinusSrcAlpha:return GL_ONE_MINUS_SRC_ALPHA;
	case AlphaBlend::DestAlpha:return GL_DST_ALPHA;
	case AlphaBlend::OneMinusDestAlpha:return GL_ONE_MINUS_DST_ALPHA;
	case AlphaBlend::DestColor:return GL_DST_COLOR;
	case AlphaBlend::OneMinusDestColor:return GL_ONE_MINUS_DST_COLOR;
	default:AssertEx(false, "%d", value); return GL_ONE;
	}
}
inline GLenum GL_GetPrimitiveType(PrimitiveType value)
{
	switch (value)
	{
	case PrimitiveType::PointList:return GL_POINTS;
	case PrimitiveType::LineList:return GL_LINES;
	case PrimitiveType::LineStrip:return GL_LINE_STRIP;
	case PrimitiveType::TriangleList:return GL_TRIANGLES;
	case PrimitiveType::TriangleStrip:return GL_TRIANGLE_STRIP;
	case PrimitiveType::TriangleFan:return GL_TRIANGLE_FAN;
	default:AssertEx(false, "%d", value); return GL_TRIANGLES;
	}
}
//缓冲区附着
static GLenum GL_GetColorAttachments(TextureFlag flags)
{
	return GL_COLOR_ATTACHMENT0;
}
static GLenum GL_GetDepthAttachments(TextureFlag flags)
{
	GLenum attachments = GL_DEPTH_ATTACHMENT;
	if (flags & TextureFlag::DEPTH || flags & TextureFlag::STENCIL)
	{
		if (flags & TextureFlag::DEPTH && flags & TextureFlag::STENCIL)
			attachments = GL_DEPTH_STENCIL_ATTACHMENT;
		else if (flags & TextureFlag::DEPTH)
			attachments = GL_DEPTH_ATTACHMENT;
		else
			attachments = GL_STENCIL_ATTACHMENT;
	}
	return attachments;
}
inline GLenum GL_GetTextureFormat(ColorFormat value)
{
	switch (value)
	{
	case ColorFormat::R8:return GL_RED;
	case ColorFormat::R5G6B5:return GL_RGB;
	case ColorFormat::R5G5B5A1:return GL_RGBA;
	case ColorFormat::R8G8B8:return GL_RGB;
	case ColorFormat::R8G8B8A8:return GL_RGBA;
	case ColorFormat::R16F:return GL_RED;
	case ColorFormat::RG16F:return GL_RG;
	case ColorFormat::RGB16F:return GL_RGB;
	case ColorFormat::RGBA16F:return GL_RGBA;
	case ColorFormat::R32F:return GL_RED;
	case ColorFormat::RG32F:return GL_RG;
	case ColorFormat::RGB32F:return GL_RGB;
	case ColorFormat::RGBA32F:return GL_RGBA;

	case ColorFormat::DEPTH16:
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:return GL_DEPTH_COMPONENT;
	case ColorFormat::DEPTH24_STENCIL8:return GL_DEPTH_STENCIL;

	case ColorFormat::ETC_RGB4:return GL_RGB;
	case ColorFormat::ETC2_RGB4:return GL_RGB;
	case ColorFormat::ETC2_RGBA8:return GL_RGBA;

	case ColorFormat::PVRTC_RGB2:return GL_RGB;
	case ColorFormat::PVRTC_RGBA2:return GL_RGBA;
	case ColorFormat::PVRTC_RGB4:return GL_RGB;
	case ColorFormat::PVRTC_RGBA4:return GL_RGBA;

	case ColorFormat::ASTC_RGB_4x4:
	case ColorFormat::ASTC_RGB_5x5:
	case ColorFormat::ASTC_RGB_6x6:
	case ColorFormat::ASTC_RGB_8x8:
	case ColorFormat::ASTC_RGB_10x10:
	case ColorFormat::ASTC_RGB_12x12:return GL_RGB;
	case ColorFormat::ASTC_RGBA_4x4:
	case ColorFormat::ASTC_RGBA_5x5:
	case ColorFormat::ASTC_RGBA_6x6:
	case ColorFormat::ASTC_RGBA_8x8:
	case ColorFormat::ASTC_RGBA_10x10:
	case ColorFormat::ASTC_RGBA_12x12:return GL_RGBA;
	default: AssertEx(false, "%d", value); return GL_RGBA;
	}
}
inline GLenum GL_GetInternalFormat(ColorFormat value, bool sRGB)
{
#if !defined(DC_COLORSPACE_LINEAR)
	sRGB = false;
#endif
	switch (value)
	{
	case ColorFormat::R8:return GL_R8;
	case ColorFormat::R5G6B5:return GL_RGB565;
	case ColorFormat::R5G5B5A1:return GL_RGB5_A1;
	case ColorFormat::R8G8B8:return sRGB ? GL_SRGB8 : GL_RGB8;
	case ColorFormat::R8G8B8A8:return sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
	case ColorFormat::R16F:return GL_R16F;
	case ColorFormat::RG16F:return GL_RG16F;
	case ColorFormat::RGB16F:return GL_RGB16F;
	case ColorFormat::RGBA16F:return GL_RGBA16F;
	case ColorFormat::R32F:return GL_R32F;
	case ColorFormat::RG32F:return GL_RG32F;
	case ColorFormat::RGB32F:return GL_RGB32F;
	case ColorFormat::RGBA32F:return GL_RGBA32F;

	case ColorFormat::DEPTH16:return GL_DEPTH_COMPONENT16;
	case ColorFormat::DEPTH24:return GL_DEPTH_COMPONENT24;
	case ColorFormat::DEPTH32F:return GL_DEPTH_COMPONENT32F;
	case ColorFormat::DEPTH24_STENCIL8:return GL_DEPTH24_STENCIL8;

	case ColorFormat::ETC_RGB4:return GL_ETC1_RGB8_OES;
	case ColorFormat::ETC2_RGB4:return sRGB ? GL_COMPRESSED_SRGB8_ETC2 : GL_COMPRESSED_RGB8_ETC2;
	case ColorFormat::ETC2_RGBA8:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC : GL_COMPRESSED_RGBA8_ETC2_EAC;

	case ColorFormat::PVRTC_RGB2:return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	case ColorFormat::PVRTC_RGBA2:return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	case ColorFormat::PVRTC_RGB4:return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	case ColorFormat::PVRTC_RGBA4:return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;

#if defined(DC_GRAPHICS_API_OPENGLES3) && defined(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4)
	case ColorFormat::ASTC_RGB_4x4:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_4x4;
	case ColorFormat::ASTC_RGB_5x5:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5 : GL_COMPRESSED_RGBA_ASTC_5x5;
	case ColorFormat::ASTC_RGB_6x6:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_6x6;
	case ColorFormat::ASTC_RGB_8x8:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_8x8;
	case ColorFormat::ASTC_RGB_10x10:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_10x10;
	case ColorFormat::ASTC_RGB_12x12:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_12x12;
	case ColorFormat::ASTC_RGBA_4x4:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4 : GL_COMPRESSED_RGBA_ASTC_4x4;
	case ColorFormat::ASTC_RGBA_5x5:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5 : GL_COMPRESSED_RGBA_ASTC_5x5;
	case ColorFormat::ASTC_RGBA_6x6:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6 : GL_COMPRESSED_RGBA_ASTC_6x6;
	case ColorFormat::ASTC_RGBA_8x8:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8 : GL_COMPRESSED_RGBA_ASTC_8x8;
	case ColorFormat::ASTC_RGBA_10x10:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10 : GL_COMPRESSED_RGBA_ASTC_10x10;
	case ColorFormat::ASTC_RGBA_12x12:return sRGB ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12 : GL_COMPRESSED_RGBA_ASTC_12x12;
#else
	case ColorFormat::ASTC_RGB_4x4:return GL_COMPRESSED_RGBA_ASTC_4x4;
	case ColorFormat::ASTC_RGB_5x5:return GL_COMPRESSED_RGBA_ASTC_5x5;
	case ColorFormat::ASTC_RGB_6x6:return GL_COMPRESSED_RGBA_ASTC_6x6;
	case ColorFormat::ASTC_RGB_8x8:return GL_COMPRESSED_RGBA_ASTC_8x8;
	case ColorFormat::ASTC_RGB_10x10:return GL_COMPRESSED_RGBA_ASTC_10x10;
	case ColorFormat::ASTC_RGB_12x12:return GL_COMPRESSED_RGBA_ASTC_12x12;
	case ColorFormat::ASTC_RGBA_4x4:return GL_COMPRESSED_RGBA_ASTC_4x4;
	case ColorFormat::ASTC_RGBA_5x5:return GL_COMPRESSED_RGBA_ASTC_5x5;
	case ColorFormat::ASTC_RGBA_6x6:return GL_COMPRESSED_RGBA_ASTC_6x6;
	case ColorFormat::ASTC_RGBA_8x8:return GL_COMPRESSED_RGBA_ASTC_8x8;
	case ColorFormat::ASTC_RGBA_10x10:return GL_COMPRESSED_RGBA_ASTC_10x10;
	case ColorFormat::ASTC_RGBA_12x12:return GL_COMPRESSED_RGBA_ASTC_12x12;
#endif
	default: AssertEx(false, "%d", value); return GL_RGBA; 
	}
}
inline GLenum GL_GetTextureFormatType(ColorFormat value)
{
	switch (value)
	{
	case ColorFormat::R8:return GL_UNSIGNED_BYTE;
	case ColorFormat::R5G6B5:return GL_UNSIGNED_SHORT_5_6_5;
	case ColorFormat::R5G5B5A1:return GL_UNSIGNED_SHORT_5_5_5_1;
	case ColorFormat::R8G8B8:return GL_UNSIGNED_BYTE;
	case ColorFormat::R8G8B8A8:return GL_UNSIGNED_BYTE;
	case ColorFormat::R16F:
	case ColorFormat::RG16F:
	case ColorFormat::RGB16F:
	case ColorFormat::RGBA16F:return GL_HALF_FLOAT;
	case ColorFormat::R32F:
	case ColorFormat::RG32F:
	case ColorFormat::RGB32F:
	case ColorFormat::RGBA32F:return GL_FLOAT;
	case ColorFormat::DEPTH16:return GL_UNSIGNED_SHORT;
	case ColorFormat::DEPTH24:return GL_UNSIGNED_INT;
	case ColorFormat::DEPTH32F:return GL_FLOAT;
	case ColorFormat::DEPTH24_STENCIL8:return GL_UNSIGNED_INT_24_8;
	default: AssertEx(false, "%d", value); return GL_UNSIGNED_BYTE;
	}
}
inline GLenum GL_GetTextureDepthFormat(ColorFormat value, TextureFlag flags)
{
	switch (value)
	{
	case ColorFormat::DEPTH16:return GL_DEPTH_COMPONENT;
	case ColorFormat::DEPTH24:return GL_DEPTH_COMPONENT;
	case ColorFormat::DEPTH32F:return GL_DEPTH_COMPONENT;
	case ColorFormat::DEPTH24_STENCIL8:return GL_DEPTH_STENCIL;
	default:
	{
		if (flags & TextureFlag::DEPTH_AND_STENCIL) return GL_DEPTH_STENCIL;
		else return GL_DEPTH_COMPONENT;
	}
	break;
	}
}
inline GLenum GL_GetInternalDepthFormat(ColorFormat value, TextureFlag flags)
{
	switch (value)
	{
	case ColorFormat::DEPTH16:return GL_DEPTH_COMPONENT16;
	case ColorFormat::DEPTH24:return GL_DEPTH_COMPONENT24;
	case ColorFormat::DEPTH32F:return GL_DEPTH_COMPONENT24;
	case ColorFormat::DEPTH24_STENCIL8:return GL_DEPTH24_STENCIL8;
	default:
	{
		if (flags & TextureFlag::DEPTH_AND_STENCIL) return GL_DEPTH24_STENCIL8;
		else return GL_DEPTH_COMPONENT16;
	}
	break;
	}
}
inline GLenum GL_GetTextureDepthFormatType(ColorFormat value, TextureFlag flags)
{
	switch (value)
	{
	case ColorFormat::DEPTH16:return GL_UNSIGNED_BYTE;
	case ColorFormat::DEPTH24:return GL_UNSIGNED_BYTE;
	case ColorFormat::DEPTH32F:return GL_UNSIGNED_BYTE;
	case ColorFormat::DEPTH24_STENCIL8:return GL_UNSIGNED_INT_24_8;
	default:
	{
		if (flags & TextureFlag::DEPTH_AND_STENCIL) return GL_UNSIGNED_INT_24_8;
		else return GL_UNSIGNED_BYTE;
	}
	break;
	}
}
inline GLenum GL_GetStencilOp(StencilOp value)
{
	switch (value)
	{
	case StencilOp::Keep:return GL_KEEP;
	case StencilOp::Zero:return GL_ZERO;
	case StencilOp::Replace: return GL_REPLACE;
	case StencilOp::Incr: return GL_INCR;
	case StencilOp::Incr_Wrap: return GL_INCR_WRAP;
	case StencilOp::Decr: return GL_DECR;
	case StencilOp::Decr_Wrap: return GL_DECR_WRAP;
	default: AssertEx(false, "%d", value); return GL_KEEP;
	}
}
inline GLenum GL_GetStencilCmp(StencilCmp value)
{
	switch (value)
	{
	case StencilCmp::Never:return GL_NEVER;
	case StencilCmp::Less:return GL_LESS;
	case StencilCmp::Equal:return GL_EQUAL;
	case StencilCmp::LEqual:return GL_LEQUAL;
	case StencilCmp::Greater:return GL_GREATER;
	case StencilCmp::NotEqual:return GL_NOTEQUAL;
	case StencilCmp::GEqual:return GL_GEQUAL;
	case StencilCmp::Always:return GL_ALWAYS;
	default:AssertEx(false, "%d", value); return GL_GREATER;
	}
}
inline GLenum GL_GetFogMode(FogMode value)
{
	switch (value)
	{
	case FogMode::Linear:return GL_LINEAR;
#if defined(DC_GRAPHICS_API_OPENGL)
	case FogMode::Exp:return GL_EXP;
	case FogMode::Exp2:return GL_EXP2;
#endif
	default: AssertEx(0, "%d", value); return GL_LINEAR;
	}
}

class GLDevice;
extern GLDevice* GetGLDevice();
DC_END_NAMESPACE


/*缓冲区
一 类型
可选参数						解释
GL_ARRAY_BUFFER					顶点缓冲区
GL_ATOMIC_COUNTER_BUFFER		同样，可以使用GL_UNIFORM_BUFFER，GL_ATOMIC_COUNTER_BUFFER和GL_SHADER_STORAGE_BUFFER缓冲区绑定点，但不分别直接影响统一缓冲区，原子计数器缓冲区或着色器存储缓冲区状态。必须使用glBindBufferBase或glBindBufferRange将缓冲区绑定到索引的统一缓冲区，原子计数器缓冲区或着色器存储缓冲区绑定点
GL_COPY_READ_BUFFER				用作通过glCopyBufferSubData进行复制的数据源
GL_COPY_WRITE_BUFFER			用作通过glCopyBufferSubData进行复制的目标
GL_DRAW_INDIRECT_BUFFER			通过glDrawArraysIndirect和glDrawElementsIndirect发出的绘制参数来自该缓冲区对象的数据存储中的指定偏移量
GL_DISPATCH_INDIRECT_BUFFER		当非零缓冲区对象绑定到GL_DISPATCH_INDIRECT_BUFFER目标时，通过glDispatchComputeIndirect发出的计算调度的参数来自该缓冲区对象的数据存储中的指定偏移量
GL_ELEMENT_ARRAY_BUFFER			索引缓冲区
GL_PIXEL_PACK_BUFFER			这一缓存目标用于从图象对象中读取数据，例如纹理和帧缓存数据，相关OPenGL命令包括glGetTexImage()和glReadPixels()等。以下命令会受到影响：glGetCompressedTexImage，glGetTexImage和glReadPixels。指针参数被解释为在基本机器单元中测量的缓冲区对象内的偏移量
GL_PIXEL_UNPACK_BUFFER			这一缓存目标与GL_PLXEL_PACK_BUFFER正好相反，它可以作为glTexImage2D()等命令的数据源使用，下面的命令会受到影响：glCompressedTexImage1D，glCompressedTexImage2D，glCompressedTexImage3D，glCompressedTexSubImage1D，glCompressedTexSubImage2D，glCompressedTexSubImage3D，glTexImage1D，glTexImage2D，glTexImage3D，glTexSubImage1D，glTexSubImage2D，和glTexSubImage3D。指针参数被解释为在基本机器单元中测量的缓冲区对象内的偏移量
GL_QUERY_BUFFER					所述GL_QUERY_BUFFER结合点被用于指定一个缓冲器对象，它是通过调用所述接收查询对象的结果glGetQueryObject家族命令
GL_SHADER_STORAGE_BUFFER		同样，可以使用GL_UNIFORM_BUFFER，GL_ATOMIC_COUNTER_BUFFER和GL_SHADER_STORAGE_BUFFER缓冲区绑定点，但不分别直接影响统一缓冲区，原子计数器缓冲区或着色器存储缓冲区状态。必须使用glBindBufferBase或glBindBufferRange将缓冲区绑定到索引的统一缓冲区，原子计数器缓冲区或着色器存储缓冲区绑定点
GL_TEXTURE_BUFFER				纹理缓存也就是直接绑定到纹理对象的缓存,这样就可以直接在着色器中读取他们的信息，GL_TEXTURE_BUFFER可以提供一个操控此类缓存的目标，但还需要将缓存关联到纹理，才能确保能够使用，glGenTextures仍然用于创建纹理对象。并且 glBindTexture仍然用于绑定纹理。对于缓冲区纹理，纹理绑定到 GL_TEXTURE_BUFFER目标。为缓冲区对象创建存储是主要区别
GL_TRANSFORM_FEEDBACK_BUFFER	transform feedback是OpenGL提供的一种便捷方案，它可以在管线的顶点处理部分结束时，将经过变换的顶点重新捕获，并将部分属性写入到缓存对象中，通常在几何着色器中使用较多，该GL_TRANSFORM_FEEDBACK_BUFFER缓冲结合点可以传递给glBindBuffer，但不会直接影响变换的反馈状态。相反，必须通过调用glBindBufferBase或glBindBufferRange来使用索引的GL_TRANSFORM_FEEDBACK_BUFFER绑定。这将影响通用GL_TRANSFORM_FEEDBACK_BUFFER绑定
GL_UNIFORM_BUFFER				这个目标可以用于创建uniform缓存对象的缓存数据，但直接影响统一存储缓冲区状态。必须使用glBindBufferBase或glBindBufferRange将缓冲区绑定到索引的统一缓冲区绑定点

二 usage标示符取以下9种值：
GL_STATIC_DRAW
GL_STATIC_READ
GL_STATIC_COPY
GL_DYNAMIC_DRAW
GL_DYNAMIC_READ
GL_DYNAMIC_COPY
GL_STREAM_DRAW
GL_STREAM_READ
GL_STREAM_COPY

"Static”意味着VBO中的数据不会被改变（一次修改，多次使用），"dynamic”意味着数据可以被频繁修改（多次修改，多次使用），"stream”意味着数据每帧都不同（一次修改，一次使用）。
"Draw”意味着数据将会被送往GPU进行绘制，"read”意味着数据会被用户的应用读取，"copy”意味着数据会被用于绘制和读取。

注意在使用VBO时，只有draw是有效的，而copy和read主要将会在像素缓冲区（PBO）和帧缓冲区（FBO）中发挥作用。

系统会根据usage标示符为缓冲区对象分配最佳的存储位置，比如系统会为GL_STATIC_DRAW和GL_STREAM_DRAW分配显存，GL_DYNAMIC_DRAW分配AGP，以及任何_READ_相关的缓冲区对象都会被存储到系统或者AGP中因为这样数据更容易读写。
*/

/*
OpenGL版本与GLSL对应关系
OpenGL	GLSL	latest spec version number	Note
1.x		固定管线
2.0	 	110		1.10.59	 
2.1		120		1.20.8	 
3.0		130		1.30.10	 
3.1		140		1.40.08	 
3.2		150		1.50.11						added Geometry Shaders
3.3		330		3.30.6	 
4.0		400		4.00.9						added both Tessellation Shaders
4.1		410		4.10.6	 
4.2		420		4.20.11	 
4.3		430		4.30.6	 					added Compute Shaders
4.4		440
4.5		450
4.6		460
*/

/*OpenglES常量寄存器数量(最小值)
版本：	ESSL		常量寄存器
1.x		(固定管线)			
2.0		100			128
3.0		300			256
3.1		310
3.2		320
3.3		330
*/
/*
OpenGL与OpenGLES，WebGL关系
1.OpenGL ES可以说是OpenGL为了满足嵌入式设备需求而开发一个特殊版本，是其一个子集；
2.而WebGL，是为了网页渲染效果，将JavaScript和OpenGL ES 2.0结合在一起，通过增加OpenGL ES 2.0的一个JavaScript绑定得到，
在实际应用过程中，前端的WebGL是通过JS语句，操作本地的OpenGL的部分接口，对于移动设备这是操作OpenGL ES的接口，来实现页面的图形的渲染，
WebGL只是绑定外面接口的一层，内部的一些核心内容，如着色器，材质，灯光等都是需要借助GLSL ES语法来操作的
版本关系:
OpenGL			1.5			2.0			3.3			4.3			4.6
OpenGLES		1.1			2.0			3.0			-			-
WebGL			-			1.0			2.0			-			-
DX				8			9			10			11			12
*/

/*
OpenGLES删除的功能：
1.glBegin/glEnd
2.glArrayElement
3.显示列表
4.求值器
5.索引色模式
6.自定义裁剪平面
7.glRect
8.图像处理(这个一般显卡也没有，FireGL/Quadro显卡有)
9.反馈缓冲
10.选择缓冲
11.累积缓冲
12.边界标志
13.glPolygonMode
14.GL_QUADS,GL_QUAD_STRIP,GL_POLYGON
15.glPushAttrib,glPopAttrib,glPushClientAttrib,glPopClientAttrib
15.TEXTURE_1D、TEXTURE_3D、TEXTURE_RECT、TEXTURE_CUBE_MAP
16.GL_COMBINE
17.自动纹理坐标生成
18.纹理边界
19.GL_CLAMP、GL_CLAMP_TO_BORDER
20.消失纹理代表
21.纹理LOD限定
22.纹理偏好限定
23.纹理自动压缩、解压缩
24.glDrawPixels,glPixelTransfer,glPixelZoom
25.glReadBuffer,glDrawBuffer,glCopyPixels
*/
