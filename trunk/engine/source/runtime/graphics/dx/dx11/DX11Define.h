 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <dwrite.h>
#include <d3dcommon.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include "../DXDefine.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

DC_BEGIN_NAMESPACE

//后台缓冲区
inline const uint			DXBackBufferCount = 3;
inline const ColorFormat	DXRenderTargetFormat = ColorFormat::R8G8B8A8;
inline const ColorFormat	DXDepthStencilFormat = ColorFormat::DEPTH24_STENCIL8;

enum D3DFeatureLevel
{
	D3DFeatureLevel_9_1 = 0,
	D3DFeatureLevel_9_2,
	D3DFeatureLevel_9_3,
	D3DFeatureLevel_10_0,
	D3DFeatureLevel_10_1,
	D3DFeatureLevel_11_0,
	D3DFeatureLevel_11_1,
	D3DFeatureLevel_12_0,
	D3DFeatureLevel_12_1,
	D3DFeatureLevelCount,
};
//根据顶点大小获得格式
inline DXGI_FORMAT DX11GetVertexType(VertexSize value)
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
inline DXGI_FORMAT DX11GetIndexType(IndexType value)
{
	if (value == IndexType::B32)
		return DXGI_FORMAT_R32_UINT;
	else
		return DXGI_FORMAT_R16_UINT;
}
inline D3D11_FILL_MODE DX11GetFillMode(FillMode value)
{
	switch (value)
	{
	case FillMode::Wire:return D3D11_FILL_WIREFRAME;
	case FillMode::Solid:return D3D11_FILL_SOLID;
	default:AssertEx(0, "%d", value); return D3D11_FILL_SOLID;
	}
}
inline D3D11_CULL_MODE DX11GetCullMode(CullMode value)
{
	switch (value)
	{
	case CullMode::Off:return D3D11_CULL_NONE;
	case CullMode::Front:return D3D11_CULL_FRONT;
	case CullMode::Back:return D3D11_CULL_BACK;
	default:AssertEx(0, "%d", value); return D3D11_CULL_NONE;
	}
}
inline D3D11_FILTER DX11GetTextureFilter(TextureFilter filter, uint anisotropy)
{
	if (anisotropy > 1)
		return D3D11_FILTER_ANISOTROPIC;//各向异性筛选（能耗最高、视觉质量最佳）

	switch (filter)
	{
	case TextureFilter::Nearest: return D3D11_FILTER_MIN_MAG_MIP_POINT;//点筛选
	case TextureFilter::Bilinear:  return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;//双线性筛选
	case TextureFilter::Trilinear:  return D3D11_FILTER_MIN_MAG_MIP_LINEAR;//三线性筛选
	default:AssertEx(0, "%d", filter); return D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
}
inline D3D11_TEXTURE_ADDRESS_MODE DX11GetTextureAddress(TextureAddress value)
{
	switch (value)
	{
	case TextureAddress::Wrap:return D3D11_TEXTURE_ADDRESS_WRAP;
	case TextureAddress::Mirror:return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TextureAddress::Clamp:return D3D11_TEXTURE_ADDRESS_CLAMP;
	case TextureAddress::Border:return D3D11_TEXTURE_ADDRESS_BORDER;
	default:AssertEx(0, "%d", value); return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}
inline D3D11_BLEND DX11GetAlphaBlend(AlphaBlend value)
{
	switch (value)
	{
	case AlphaBlend::Zero:return D3D11_BLEND_ZERO;
	case AlphaBlend::One:return D3D11_BLEND_ONE;
	case AlphaBlend::SrcColor:return D3D11_BLEND_SRC_COLOR;
	case AlphaBlend::OneMinusSrcColor:return D3D11_BLEND_INV_SRC_COLOR;
	case AlphaBlend::SrcAlpha:return D3D11_BLEND_SRC_ALPHA;
	case AlphaBlend::OneMinusSrcAlpha:return D3D11_BLEND_INV_SRC_ALPHA;
	case AlphaBlend::DestAlpha:return D3D11_BLEND_DEST_ALPHA;
	case AlphaBlend::OneMinusDestAlpha:return D3D11_BLEND_INV_DEST_ALPHA;
	case AlphaBlend::DestColor:return D3D11_BLEND_DEST_COLOR;
	case AlphaBlend::OneMinusDestColor:return D3D11_BLEND_INV_DEST_COLOR;
	default:AssertEx(0, "%d", value); return D3D11_BLEND_ONE;
	}
}
inline D3D11_COMPARISON_FUNC DX11GetStencilCmp(StencilCmp value)
{
	switch (value)
	{
	case StencilCmp::Never:return D3D11_COMPARISON_NEVER;
	case StencilCmp::Less:return D3D11_COMPARISON_LESS;
	case StencilCmp::Equal:return D3D11_COMPARISON_EQUAL;
	case StencilCmp::LessEqual:return D3D11_COMPARISON_LESS_EQUAL;
	case StencilCmp::Greater:return D3D11_COMPARISON_GREATER;
	case StencilCmp::NotEqual:return D3D11_COMPARISON_NOT_EQUAL;
	case StencilCmp::GreaterEqual:return D3D11_COMPARISON_GREATER_EQUAL;
	case StencilCmp::Always:return D3D11_COMPARISON_ALWAYS;
	default:AssertEx(0, "%d", value); return D3D11_COMPARISON_GREATER;
	}
}
inline D3D11_STENCIL_OP DX11GetStencilOp(StencilOp value)
{
	switch (value)
	{
	case StencilOp::Keep:return D3D11_STENCIL_OP_KEEP;
	case StencilOp::Zero:return D3D11_STENCIL_OP_ZERO;
	case StencilOp::Replace: return  D3D11_STENCIL_OP_REPLACE;
	case StencilOp::Incr: return D3D11_STENCIL_OP_INCR_SAT;
	case StencilOp::Incr_Wrap: return D3D11_STENCIL_OP_INCR;
	case StencilOp::Decr: return D3D11_STENCIL_OP_DECR_SAT;
	case StencilOp::Decr_Wrap: return D3D11_STENCIL_OP_DECR;
	default: AssertEx(0, "%d", value); return D3D11_STENCIL_OP_KEEP;
	}
}
inline D3D_PRIMITIVE_TOPOLOGY DX11GetPrimitiveType(PrimitiveType value)
{
	switch (value)
	{
	case PrimitiveType::PointList:return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveType::LineList:return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveType::LineStrip:return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveType::TriangleList:return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveType::TriangleStrip:return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:AssertEx(0, "%d", value); return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

//获得颜色格式，TODO:不支持R8G8B8和R5G6B5
inline DXGI_FORMAT DX11GetTextureFormat(ColorFormat format, bool sRGB)
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
inline DXGI_FORMAT DX11GetTextureViewFormat(ColorFormat format, bool sRGB)
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
inline DXGI_FORMAT DX11GetShaderViewFormat(ColorFormat format, bool sRGB)
{
	switch (format)
	{
	case ColorFormat::DEPTH16: return DXGI_FORMAT_R16_UNORM;
	case ColorFormat::DEPTH24_STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	default: return DX11GetTextureViewFormat(format, sRGB);
	}
}
//深度
inline DXGI_FORMAT DX11GetDepthTextureFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_R24G8_TYPELESS;
	default: return DXGI_FORMAT_R16_TYPELESS;
	}
}
inline DXGI_FORMAT DX11GetDepthTextureViewFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8:return DXGI_FORMAT_D24_UNORM_S8_UINT;
	default: return DXGI_FORMAT_D16_UNORM;
	}
}
inline DXGI_FORMAT DX11GetDepthShaderViewFormat(ColorFormat format)
{
	switch (format)
	{
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:
	case ColorFormat::DEPTH24_STENCIL8: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	default: return DXGI_FORMAT_R16_UNORM;
	}
}

inline ColorFormat DX11GetColorFormat(DXGI_FORMAT value)
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
inline DXGI_FORMAT DX11EnsureNotTypeless(DXGI_FORMAT fmt)
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

class DX11Device;
extern DX11Device* GetDX11Device();

//编译shader
extern bool DX11CompileShaderFromFile(const String& file, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer);
extern bool DX11CompileShader(const String& name, const String& code, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer);
DC_END_NAMESPACE

/*BC格式
1．BC1（DXGI_FORMAT_BC1_UNORM）：该格式支持3个颜色通道，仅用1位（开/关）表示alpha分量。
2．BC2（DXGI_FORMAT_BC2_UNORM）：该格式支持3个颜色通道，仅用4位表示alpha分量。
3．BC3（DXGI_FORMAT_BC3_UNORM）：该格式支持3个颜色通道，以8位表示alpha分量。
4．BC4（DXGI_FORMAT_BC4_UNORM）：该格式支持1个颜色通道（例如，灰阶图像）。
5．BC5（DXGI_FORMAT_BC5_UNORM）：该格式支持两个颜色通道。
6．BC6（DXGI_FORMAT_BC6_UF16） ：该格式用于压缩的HDR（高动态范围，high dynamic range）图像数据。
7．BC7（DXGI_FORMAT_BC7_UNORM）：该格式用于高质量的RGBA压缩。特别的有，这个格式可以显著地减少由于压缩法线贴图带来的错误。
*/