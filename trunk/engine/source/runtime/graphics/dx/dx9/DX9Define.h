
/*****************************************************************************
* Author： hannibal
* Date：2009年11月25日
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#ifdef DC_DEBUG
	#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include "../DXDefine.h"
#include "runtime/graphics/RenderDefine.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#if defined(DC_DEBUG)
	#include <DxErr.h>
	#pragma comment(lib,"DxErr.lib")
#endif

DC_BEGIN_NAMESPACE

//后台缓冲区
inline uint			DX9BackBufferCount = 1;
inline ColorFormat	DX9RenderTargetFormat = ColorFormat::R8G8B8A8;
inline ColorFormat	DX9DepthStencilFormat = ColorFormat::DEPTH24_STENCIL8;

//事件
inline const String PreLostDeviceEvt = "PreLostDeviceEvt";
inline const String LostDeviceEvt = "LostDeviceEvt";
inline const String ResetDeviceEvt = "ResetDeviceEvt";

inline D3DPOOL DX9GetResourcePool(GPUResourcePool value)
{
	D3DPOOL kD3DPool; 
	switch (value)
	{
	case GPUResourcePool::Default:kD3DPool = D3DPOOL_DEFAULT; break;
	case GPUResourcePool::Manager:kD3DPool = D3DPOOL_MANAGED; break;
	case GPUResourcePool::System:kD3DPool = D3DPOOL_SYSTEMMEM; break;
	case GPUResourcePool::Scratch:kD3DPool = D3DPOOL_SCRATCH; break;
	default:AssertEx(0, "%d", value); kD3DPool = D3DPOOL_MANAGED; break;
	}
	return kD3DPool;
}
inline DWORD DX9GetResourceLock(GPUResourceLock value)
{
	DWORD kD3DLock;
	switch (value)
	{
	case GPUResourceLock::Discard:kD3DLock = D3DLOCK_DISCARD; break;
	case GPUResourceLock::NoOverWrite:kD3DLock = D3DLOCK_NOOVERWRITE; break;
	case GPUResourceLock::ReadOnly:kD3DLock = D3DLOCK_READONLY; break;
	default:AssertEx(0, "%d", value); kD3DLock = D3DLOCK_DISCARD; break;
	}
	return kD3DLock;
}
inline DWORD DX9GetResourceUsage(GPUResourceUsage value)
{
	DWORD kD3DUsage;
	switch (value)
	{
	case GPUResourceUsage::Dynamic:kD3DUsage = D3DUSAGE_DYNAMIC; break;
	case GPUResourceUsage::Static:kD3DUsage = D3DUSAGE_WRITEONLY; break;
	default:AssertEx(0, "%d", value); kD3DUsage = D3DUSAGE_DYNAMIC; break;
	}
	return kD3DUsage;
}
inline D3DDECLTYPE DX9GetVertexDeclType(VertexSize value)
{
	switch (value)
	{
	case VertexSize::Color:return D3DDECLTYPE_D3DCOLOR;
	case VertexSize::Float1:return D3DDECLTYPE_FLOAT1;
	case VertexSize::Float2:return D3DDECLTYPE_FLOAT2;
	case VertexSize::Float3:return D3DDECLTYPE_FLOAT3;
	case VertexSize::Float4:return D3DDECLTYPE_FLOAT4;
	case VertexSize::Short2:return D3DDECLTYPE_SHORT2;
	case VertexSize::Short4:return D3DDECLTYPE_SHORT4;
	case VertexSize::Byte4:return D3DDECLTYPE_UBYTE4;
	default:AssertEx(0, "%d", value); return D3DDECLTYPE_FLOAT3;
	}
}
inline D3DDECLUSAGE DX9GetVertexDeclUsage(VertexSemantic value)
{
	switch (value)
	{
	case VertexSemantic::Position:return D3DDECLUSAGE_POSITION;
	case VertexSemantic::Diffuse:return D3DDECLUSAGE_COLOR;
	case VertexSemantic::Normal:return D3DDECLUSAGE_NORMAL;
	case VertexSemantic::Tangent:return D3DDECLUSAGE_TANGENT;
	case VertexSemantic::Coordinates1:return D3DDECLUSAGE_TEXCOORD;
	case VertexSemantic::Coordinates2:return D3DDECLUSAGE_TEXCOORD;
	case VertexSemantic::Indices:return D3DDECLUSAGE_BLENDINDICES;
	case VertexSemantic::Weights:return D3DDECLUSAGE_BLENDWEIGHT;
	case VertexSemantic::Size:return D3DDECLUSAGE_PSIZE;
	case VertexSemantic::InstanceOffset:return D3DDECLUSAGE_POSITION;
	default:AssertEx(0, "%d", value); return D3DDECLUSAGE_POSITION;
	}
}
inline D3DFORMAT DX9GetIndexType(IndexType value)
{
	if (value == IndexType::B32)
		return D3DFMT_INDEX32;
	else
		return D3DFMT_INDEX16;
}
inline D3DFILLMODE DX9GetFillMode(FillMode value)
{
	switch (value)
	{
	case FillMode::Point:return D3DFILL_POINT;
	case FillMode::Wire:return D3DFILL_WIREFRAME;
	case FillMode::Solid:return D3DFILL_SOLID;
	default:AssertEx(0, "%d", value); return D3DFILL_SOLID;
	}
}
inline D3DSHADEMODE DX9GetShadeMode(ShadeMode value)
{
	switch (value)
	{
	case ShadeMode::Flat:return D3DSHADE_FLAT;
	case ShadeMode::Gouraud:return D3DSHADE_GOURAUD;
	default:AssertEx(0, "%d", value); return D3DSHADE_GOURAUD;
	}
}
inline D3DCULL DX9GetCullMode(CullMode value)
{
	switch (value)
	{
	case CullMode::Off:return D3DCULL_NONE;
	case CullMode::Front:return D3DCULL_CW;
	case CullMode::Back:return D3DCULL_CCW;
	default:AssertEx(0, "%d", value); return D3DCULL_NONE;
	}
}
inline D3DTEXTUREFILTERTYPE DX9GetTextureFilter(TextureFilter value, int aniso)
{
	if (aniso > 1)return D3DTEXF_ANISOTROPIC;

	switch (value)
	{
	case TextureFilter::Nearest:return D3DTEXF_POINT;
	case TextureFilter::Bilinear:return D3DTEXF_LINEAR;
	case TextureFilter::Trilinear:return D3DTEXF_LINEAR;
	default:AssertEx(0, "%d", value); return D3DTEXF_LINEAR;
	}
}
inline D3DTEXTUREFILTERTYPE DX9GetTextureMipFilter(TextureFilter value)
{
	switch (value)
	{
	case TextureFilter::Nearest:return D3DTEXF_NONE;
	case TextureFilter::Bilinear:return D3DTEXF_POINT;
	case TextureFilter::Trilinear:return D3DTEXF_LINEAR;
	default:AssertEx(0, "%d", value); return D3DTEXF_POINT;
	}
}
inline D3DTEXTUREADDRESS DX9GetTextureAddress(TextureAddress value)
{
	switch (value)
	{
	case TextureAddress::Wrap:return D3DTADDRESS_WRAP;
	case TextureAddress::Mirror:return D3DTADDRESS_MIRROR;
	case TextureAddress::Clamp:return D3DTADDRESS_CLAMP;
	case TextureAddress::Border:return D3DTADDRESS_BORDER;
	default:AssertEx(0, "%d", value); return D3DTADDRESS_CLAMP;
	}
}
inline D3DBLEND DX9GetAlphaBlend(AlphaBlend value)
{
	switch (value)
	{
	case AlphaBlend::Zero:return D3DBLEND_ZERO;
	case AlphaBlend::One:return D3DBLEND_ONE;
	case AlphaBlend::SrcColor:return D3DBLEND_SRCCOLOR;
	case AlphaBlend::OneMinusSrcColor:return D3DBLEND_INVSRCCOLOR;
	case AlphaBlend::SrcAlpha:return D3DBLEND_SRCALPHA;
	case AlphaBlend::OneMinusSrcAlpha:return D3DBLEND_INVSRCALPHA;
	case AlphaBlend::DestAlpha:return D3DBLEND_DESTALPHA;
	case AlphaBlend::OneMinusDestAlpha:return D3DBLEND_INVDESTALPHA;
	case AlphaBlend::DestColor:return D3DBLEND_DESTCOLOR;
	case AlphaBlend::OneMinusDestColor:return D3DBLEND_INVDESTCOLOR;
	default:AssertEx(0, "%d", value); return D3DBLEND_ONE;
	}
}
inline D3DCMPFUNC DX9GetStencilCmp(StencilCmp value)
{
	switch (value)
	{
	case StencilCmp::Never:return D3DCMP_NEVER;
	case StencilCmp::Less:return D3DCMP_LESS;
	case StencilCmp::Equal:return D3DCMP_EQUAL;
	case StencilCmp::LEqual:return D3DCMP_LESSEQUAL;
	case StencilCmp::Greater:return D3DCMP_GREATER;
	case StencilCmp::NotEqual:return D3DCMP_NOTEQUAL;
	case StencilCmp::GEqual:return D3DCMP_GREATEREQUAL;
	case StencilCmp::Always:return D3DCMP_ALWAYS;
	default:AssertEx(0, "%d", value); return D3DCMP_GREATER;
	}
}
inline DWORD DX9GetStencilOp(StencilOp value)
{
	switch (value)
	{
	case StencilOp::Keep:return D3DSTENCILOP_KEEP;
	case StencilOp::Zero:return D3DSTENCILOP_ZERO;
	case StencilOp::Replace: return  D3DSTENCILOP_REPLACE;
	case StencilOp::Incr: return D3DSTENCILOP_INCRSAT;
	case StencilOp::Incr_Wrap: return D3DSTENCILOP_INCR;
	case StencilOp::Decr: return D3DSTENCILOP_DECRSAT;
	case StencilOp::Decr_Wrap: return D3DSTENCILOP_DECR;
	default: AssertEx(0, "%d", value); return D3DSTENCILOP_KEEP;
	}
}
inline D3DPRIMITIVETYPE DX9GetPrimitiveType(PrimitiveType value)
{
	switch (value)
	{
	case PrimitiveType::PointList:return D3DPT_POINTLIST;
	case PrimitiveType::LineList:return D3DPT_LINELIST;
	case PrimitiveType::LineStrip:return D3DPT_LINESTRIP;
	case PrimitiveType::TriangleList:return D3DPT_TRIANGLELIST;
	case PrimitiveType::TriangleStrip:return D3DPT_TRIANGLESTRIP;
	case PrimitiveType::TriangleFan:return D3DPT_TRIANGLEFAN;
	default:AssertEx(0, "%d", value); return D3DPT_TRIANGLELIST;
	}
}
inline D3DFORMAT DX9GetTextureFormat(ColorFormat value)
{
	switch (value)
	{
	case ColorFormat::R5G6B5:return D3DFMT_R5G6B5;
	case ColorFormat::R5G5B5A1:return D3DFMT_A1R5G5B5;
	case ColorFormat::R8G8B8:return D3DFMT_X8R8G8B8;
	case ColorFormat::R8G8B8A8:return D3DFMT_A8R8G8B8;
	case ColorFormat::R16F:return D3DFMT_R16F;
	case ColorFormat::RG16F:return D3DFMT_G16R16F;
	case ColorFormat::RGBA16F:return D3DFMT_A16B16G16R16F;
	case ColorFormat::R32F:return D3DFMT_R32F;
	case ColorFormat::RG32F:return D3DFMT_G32R32F;
	case ColorFormat::RGBA32F:return D3DFMT_A32B32G32R32F;
	case ColorFormat::DEPTH16:return D3DFMT_D16;
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:return D3DFMT_R32F;
	case ColorFormat::DEPTH24_STENCIL8:return D3DFMT_D24S8;
	case ColorFormat::DXT1:return D3DFMT_DXT1;
	case ColorFormat::DXT3:return D3DFMT_DXT3;
	case ColorFormat::DXT5:return D3DFMT_DXT5;
	default: AssertEx(0, "%d", value); return D3DFMT_UNKNOWN;
	}
}
inline D3DFORMAT DX9GetRenderTargetFormat(ColorFormat value)
{
	switch (value)
	{
	case ColorFormat::R5G6B5:return D3DFMT_R5G6B5;
	case ColorFormat::R5G5B5A1:return D3DFMT_A1R5G5B5;
	case ColorFormat::R8G8B8:return D3DFMT_X8R8G8B8;
	case ColorFormat::R8G8B8A8:return D3DFMT_A8R8G8B8;
	case ColorFormat::R16F:return D3DFMT_R16F;
	case ColorFormat::RG16F:return D3DFMT_G16R16F;
	case ColorFormat::RGBA16F:return D3DFMT_A16B16G16R16F;
	case ColorFormat::R32F:return D3DFMT_R32F;
	case ColorFormat::RG32F:return D3DFMT_G32R32F;
	case ColorFormat::RGBA32F:return D3DFMT_A32B32G32R32F;
	case ColorFormat::DEPTH16:return D3DFMT_R16F;
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F:return D3DFMT_R32F;
	case ColorFormat::DEPTH24_STENCIL8:return D3DFMT_R32F;
	default: AssertEx(0, "%d", value); return D3DFMT_UNKNOWN;
	}
}
inline D3DFORMAT DX9GetDepthStencilFormat(ColorFormat value, TextureFlag flags)
{
	switch (value)
	{
	case ColorFormat::DEPTH16: return D3DFMT_D16;
	case ColorFormat::DEPTH24:
	case ColorFormat::DEPTH32F: return D3DFMT_D24X8;
	case ColorFormat::DEPTH24_STENCIL8:return D3DFMT_D24S8;
	default: 
	{
		if(flags & TextureFlag::DEPTH_AND_STENCIL) return D3DFMT_D24S8;
		else return D3DFMT_D16;
	}
	break;
	}
}
inline D3DFORMAT DX9GetBackbufferFormat(ColorFormat value)
{
	switch (value)
	{
	case ColorFormat::R5G5B5A1:;
	case ColorFormat::R5G6B5:return D3DFMT_R5G6B5;
	case ColorFormat::R8G8B8:return D3DFMT_X8R8G8B8;
	case ColorFormat::R8G8B8A8:return D3DFMT_X8R8G8B8;
	default: AssertEx(0, "%d", value); return D3DFMT_UNKNOWN;
	}
}
inline D3DFORMAT DX9ConvertToAlphaFormat(D3DFORMAT fmt)
{
	switch (fmt)
	{
	case D3DFMT_X8R8G8B8:return D3DFMT_A8R8G8B8;
	case D3DFMT_X4R4G4B4:return D3DFMT_A4R4G4B4;
	case D3DFMT_X1R5G5B5:return D3DFMT_A1R5G5B5;
	default: return fmt;
	}
}
inline ColorFormat DX9GetColorFormat(D3DFORMAT value)
{
	switch (value)
	{
	case D3DFMT_A1R5G5B5:return ColorFormat::R5G5B5A1;
	case D3DFMT_R5G6B5:return ColorFormat::R5G6B5;
	case D3DFMT_R8G8B8: return ColorFormat::R8G8B8;
	case D3DFMT_A8R8G8B8:return ColorFormat::R8G8B8A8;
	case D3DFMT_X8R8G8B8:return ColorFormat::R8G8B8A8;
	case D3DFMT_R32F:return ColorFormat::R32F;
	case D3DFMT_R16F:return ColorFormat::R16F;
	default: AssertEx(0, "%d", value); return ColorFormat::R8G8B8A8;
	}
}
inline D3DXIMAGE_FILEFORMAT DX9GetImageType(ImageType value)
{
	switch (value)
	{
	case ImageType::BMP:return D3DXIFF_BMP;
	case ImageType::JPEG:return D3DXIFF_JPG;
	case ImageType::TGA: return D3DXIFF_TGA;
	case ImageType::PNG:return D3DXIFF_PNG;
	case ImageType::DDS:return D3DXIFF_DDS;
	case ImageType::HDR:return D3DXIFF_HDR;
	default: AssertEx(0, "%d", value); return D3DXIFF_PNG;
	}
}
inline DWORD DX9GetFogMode(FogMode value)
{
	switch (value)
	{
	case FogMode::Linear:return D3DFOG_LINEAR;
	case FogMode::Exp:return D3DFOG_EXP;
	case FogMode::Exp2:return D3DFOG_EXP2;
	default: AssertEx(0, "%d", value); return D3DFOG_LINEAR;
	}
}
inline D3DCOLOR DX9ToColor(const Color& color)
{
	return D3DCOLOR_ARGB(int(color.a * 255), int(color.r * 255), int(color.g * 255), int(color.b * 255));
}
/********************************************************************/
inline const char* DX9GetError(HRESULT hr)
{
#if defined(DC_DEBUG)
	return DXGetErrorDescriptionA(hr);
#else
	return "";
#endif
}

#if DC_DEBUG
	#define DX9HR(hr) do { AssertEx((SUCCEEDED(hr) || GetDX9Device()->IsDeviceInLostState()), "%s", DX9GetError(hr)); } while(0)
#else
	#define DX9HR(hr) UNUSED(hr)
#endif

class DX9Device;
extern DX9Device* GetDX9Device();
DC_END_NAMESPACE

/*
SM3.0语义
顶点着色器可以接受的输入：
BINORMAL[n]		Binormal						float4
BLENDINDICES[n] Blend indices					uint
BLENDWEIGHT[n]	Blend weights					float
COLOR[n]		Diffuse and specular color		float4
NORMAL[n]		Normal vector					float4
POSITION[n]		Vertex position in object space float4
POSITIONT		Transformed vertex position.	float4
PSIZE[n]		Point size 						float
TANGENT[n]		Tangent 						float4
TEXCOORD[n]		Texture coordinates 			float4

顶点着色器可以接受的输出：
COLOR[n]		Diffuse or specular color		float4
FOG				Vertex fog 						float
POSITION[n]		Position of a vertex in homogenous space. Compute position in screen-space by dividing (x,y,z) by w. Every vertex shader must write out a parameter with this semantic. float4
PSIZE			Point size 						float
TESSFACTOR[n]	Tessellation factor				float
TEXCOORD[n]		Texture coordinates				float4

像素着色器可以接受的输入：
COLOR[n]		Diffuse or specular color.		float4
TEXCOORD[n]		Texture coordinates				float4
VFACE			Floating-point scalar that indicates a back-facing primitive. A negative value faces backwards, while a positive value faces the camera. float
VPOS			The pixel location (x,y) in screen space. To convert a Direct3D 9 shader (that uses this semantic) to a Direct3D 10 shader, see Direct3D 9 VPOS and Direct3D 10 SV_Position) float2

像素着色器可以接受的输出：
COLOR[n]		Output color 					float4
DEPTH[n]		Output depth 					float

*/