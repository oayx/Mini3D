 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_4.h>
#include <dwrite.h>
#include <d3d11shader.h>
#include "../DX10Define.h"

#pragma comment(lib, "d3d11.lib")

DC_BEGIN_NAMESPACE

//后台缓冲区
inline const uint			DX11BackBufferCount = 3;
inline const ColorFormat	DX11RenderTargetFormat = ColorFormat::R8G8B8A8;
inline const ColorFormat	DX11DepthStencilFormat = ColorFormat::DEPTH24_STENCIL8;


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
	case StencilCmp::LEqual:return D3D11_COMPARISON_LESS_EQUAL;
	case StencilCmp::Greater:return D3D11_COMPARISON_GREATER;
	case StencilCmp::NotEqual:return D3D11_COMPARISON_NOT_EQUAL;
	case StencilCmp::GEqual:return D3D11_COMPARISON_GREATER_EQUAL;
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

class DX11Device;
extern DX11Device* GetDX11Device();

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