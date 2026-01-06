 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include "DXDefine.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

DC_BEGIN_NAMESPACE

/********************************************************************/
//根据顶点大小获得格式
inline DXGI_FORMAT DX10GetVertexType(VertexSize value)
{
	switch (value)
	{
	case VertexSize::Color:return DXGI_FORMAT_R8G8B8A8_UNORM;
	case VertexSize::Float1:return DXGI_FORMAT_R32_FLOAT;
	case VertexSize::Float2:return DXGI_FORMAT_R32G32_FLOAT;
	case VertexSize::Float3:return DXGI_FORMAT_R32G32B32_FLOAT;
	case VertexSize::Float4:return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case VertexSize::Short2:return DXGI_FORMAT_R16G16_UINT;
	case VertexSize::Short4:return DXGI_FORMAT_R16G16B16A16_UINT;
	case VertexSize::Byte4:return DXGI_FORMAT_R8G8B8A8_UNORM;
	default:AssertEx(0, "%d", value); return DXGI_FORMAT_R32G32B32_FLOAT;
	}
}
inline DXGI_FORMAT DX10GetIndexType(IndexType value)
{
	if (value == IndexType::B32)
		return DXGI_FORMAT_R32_UINT;
	else
		return DXGI_FORMAT_R16_UINT;
}

//获得颜色格式，TODO:不支持R8G8B8和R5G6B5
inline DXGI_FORMAT DX10GetTextureFormat(ColorFormat format, bool sRGB)
{
#if !defined(DC_COLORSPACE_LINEAR)
	sRGB = false;
#endif
	switch (format)
	{
	case ColorFormat::R8:return DXGI_FORMAT_R8_UNORM;
	case ColorFormat::R5G6B5:
	case ColorFormat::R8G8B8:return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	case ColorFormat::R5G5B5A1:
	case ColorFormat::R8G8B8A8:return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	case ColorFormat::R16F:return DXGI_FORMAT_R16_TYPELESS;
	case ColorFormat::RG16F:return DXGI_FORMAT_R16G16_TYPELESS;
	case ColorFormat::RGB16F:return DXGI_FORMAT_UNKNOWN;
	case ColorFormat::RGBA16F:return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case ColorFormat::R32F:return DXGI_FORMAT_R32_TYPELESS;
	case ColorFormat::RG32F:return DXGI_FORMAT_R32G32_TYPELESS;
	case ColorFormat::RGB32F:return DXGI_FORMAT_R32G32B32_TYPELESS;
	case ColorFormat::RGBA32F:return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case ColorFormat::DEPTH16:return DXGI_FORMAT_R16_TYPELESS;
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:return DXGI_FORMAT_R32_TYPELESS;
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_R24G8_TYPELESS;
	case ColorFormat::DXT1:return sRGB ? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM;
	case ColorFormat::DXT3:return sRGB ? DXGI_FORMAT_BC2_UNORM_SRGB : DXGI_FORMAT_BC2_UNORM;
	case ColorFormat::DXT5:return sRGB ? DXGI_FORMAT_BC3_UNORM_SRGB : DXGI_FORMAT_BC3_UNORM;
	default: AssertEx(0, "%d", format); return DXGI_FORMAT_UNKNOWN;
	}
}
inline DXGI_FORMAT DX10GetTextureViewFormat(ColorFormat format, bool sRGB)
{
#if !defined(DC_COLORSPACE_LINEAR)
	sRGB = false;
#endif
	switch (format)
	{
	case ColorFormat::R8:return DXGI_FORMAT_R8_UNORM;
	case ColorFormat::R5G6B5:
	case ColorFormat::R8G8B8:return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	case ColorFormat::R5G5B5A1:
	case ColorFormat::R8G8B8A8:return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	case ColorFormat::R16F:return DXGI_FORMAT_R16_FLOAT;
	case ColorFormat::RG16F:return DXGI_FORMAT_R16G16_FLOAT;
	case ColorFormat::RGB16F:return DXGI_FORMAT_UNKNOWN;
	case ColorFormat::RGBA16F:return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case ColorFormat::R32F:return DXGI_FORMAT_R32_FLOAT;
	case ColorFormat::RG32F:return DXGI_FORMAT_R32G32_FLOAT;
	case ColorFormat::RGB32F:return DXGI_FORMAT_R32G32B32_FLOAT;
	case ColorFormat::RGBA32F:return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ColorFormat::DEPTH16:return DXGI_FORMAT_D16_UNORM;
	case ColorFormat::DEPTH24:return DXGI_FORMAT_UNKNOWN;
	case ColorFormat::DEPTH32F:return DXGI_FORMAT_R32_FLOAT;
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case ColorFormat::DXT1:return sRGB ? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM;
	case ColorFormat::DXT3:return sRGB ? DXGI_FORMAT_BC2_UNORM_SRGB : DXGI_FORMAT_BC2_UNORM;
	case ColorFormat::DXT5:return sRGB ? DXGI_FORMAT_BC3_UNORM_SRGB : DXGI_FORMAT_BC3_UNORM;
	default: AssertEx(0, "%d", format); return DXGI_FORMAT_UNKNOWN;
	}
}
inline DXGI_FORMAT DX10GetShaderViewFormat(ColorFormat format, bool sRGB)
{
	switch (format)
	{
	case ColorFormat::DEPTH16: return DXGI_FORMAT_R16_UNORM;
	case ColorFormat::DEPTH24_STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	default: return DX10GetTextureViewFormat(format, sRGB);
	}
}
//深度
inline DXGI_FORMAT DX10GetDepthTextureFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_R24G8_TYPELESS;
	default: return DXGI_FORMAT_R16_TYPELESS;
	}
}
inline DXGI_FORMAT DX10GetDepthTextureViewFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_D24_UNORM_S8_UINT;
	default: return DXGI_FORMAT_D16_UNORM;
	}
}
inline DXGI_FORMAT DX10GetDepthShaderViewFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	default: return DXGI_FORMAT_R16_UNORM;
	}
}

inline ColorFormat DX10GetColorFormat(DXGI_FORMAT value)
{
	switch (value)
	{
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_SNORM:return ColorFormat::R8;
	case DXGI_FORMAT_B5G6R5_UNORM:return ColorFormat::B5G6R5;
	case DXGI_FORMAT_B5G5R5A1_UNORM:return ColorFormat::B5G5R5A1;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:return ColorFormat::B8G8R8;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:return ColorFormat::B8G8R8A8;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:return ColorFormat::R8G8B8A8;
	case DXGI_FORMAT_R16_UNORM:return ColorFormat::R16;
	case DXGI_FORMAT_R16_FLOAT:return ColorFormat::R16F;
	case DXGI_FORMAT_R16G16_FLOAT:return ColorFormat::RG16F;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:return ColorFormat::RGBA16F;
	case DXGI_FORMAT_R32_FLOAT:return ColorFormat::R32F;
	case DXGI_FORMAT_R32G32_FLOAT:return ColorFormat::RG32F;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:return ColorFormat::RGBA32F;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:return ColorFormat::DEPTH24_STENCIL8;
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:return ColorFormat::DXT1;
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:return ColorFormat::DXT3;
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:return ColorFormat::DXT5;
	default: AssertEx(0, "%d", value); return ColorFormat::R8G8B8A8;
	}
}
inline DXGI_FORMAT DX10EnsureNotTypeless(DXGI_FORMAT fmt)
{
	// Assumes UNORM or FLOAT; doesn't use UINT or SINT
	switch (fmt)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
	case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
	case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
	case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
	case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
	case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
	case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
	case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
	case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
	case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
	case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
	case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
	case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
	default:                                return fmt;
	}
}

//编译shader
extern bool DX10CompileShaderFromFile(const String& file, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer);
extern bool DX10CompileShader(const String& name, const String& code, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer);
DC_END_NAMESPACE