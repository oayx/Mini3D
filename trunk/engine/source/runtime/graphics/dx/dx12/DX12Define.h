 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include <d3d11.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <d3d12shader.h>
#include "../DX10Define.h"

#pragma comment(lib, "d3d12.lib")

DC_BEGIN_NAMESPACE

//后台缓冲区
inline const uint			DX12BackBufferCount = 3;
inline const ColorFormat	DX12RenderTargetFormat = ColorFormat::R8G8B8A8;
inline const ColorFormat	DX12DepthStencilFormat = ColorFormat::DEPTH24_STENCIL8;

/********************************************************************/
inline D3D12_FILL_MODE DX12GetFillMode(FillMode value)
{
	switch (value)
	{
	case FillMode::Wire:return D3D12_FILL_MODE_WIREFRAME;
	case FillMode::Solid:return D3D12_FILL_MODE_SOLID;
	default:AssertEx(0, "%d", value); return D3D12_FILL_MODE_SOLID;
	}
}
inline D3D12_CULL_MODE DX12GetCullMode(CullMode value)
{
	switch (value)
	{
	case CullMode::Off:return D3D12_CULL_MODE_NONE;
	case CullMode::Front:return D3D12_CULL_MODE_FRONT;
	case CullMode::Back:return D3D12_CULL_MODE_BACK;
	default:AssertEx(0, "%d", value); return D3D12_CULL_MODE_NONE;
	}
}
inline D3D12_FILTER DX12GetTextureFilter(TextureFilter filter, uint anisotropy)
{
	if (anisotropy > 1)
		return D3D12_FILTER_ANISOTROPIC;//各向异性筛选（能耗最高、视觉质量最佳）

	switch (filter)
	{
	case TextureFilter::Nearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;//点筛选
	case TextureFilter::Bilinear:  return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;//双线性筛选
	case TextureFilter::Trilinear:  return D3D12_FILTER_MIN_MAG_MIP_LINEAR;//三线性筛选
	default:AssertEx(0, "%d", filter); return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
}
inline D3D12_TEXTURE_ADDRESS_MODE DX12GetTextureAddress(TextureAddress value)
{
	switch (value)
	{
	case TextureAddress::Wrap:return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case TextureAddress::Mirror:return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case TextureAddress::Clamp:return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case TextureAddress::Border:return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	default:AssertEx(0, "%d", value); return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
}
inline D3D12_BLEND DX12GetAlphaBlend(AlphaBlend value)
{
	switch (value)
	{
	case AlphaBlend::Zero:return D3D12_BLEND_ZERO;
	case AlphaBlend::One:return D3D12_BLEND_ONE;
	case AlphaBlend::SrcColor:return D3D12_BLEND_SRC_COLOR;
	case AlphaBlend::OneMinusSrcColor:return D3D12_BLEND_INV_SRC_COLOR;
	case AlphaBlend::SrcAlpha:return D3D12_BLEND_SRC_ALPHA;
	case AlphaBlend::OneMinusSrcAlpha:return D3D12_BLEND_INV_SRC_ALPHA;
	case AlphaBlend::DestAlpha:return D3D12_BLEND_DEST_ALPHA;
	case AlphaBlend::OneMinusDestAlpha:return D3D12_BLEND_INV_DEST_ALPHA;
	case AlphaBlend::DestColor:return D3D12_BLEND_DEST_COLOR;
	case AlphaBlend::OneMinusDestColor:return D3D12_BLEND_INV_DEST_COLOR;
	default:AssertEx(0, "%d", value); return D3D12_BLEND_ONE;
	}
}
inline D3D12_COMPARISON_FUNC DX12GetStencilCmp(StencilCmp value)
{
	switch (value)
	{
	case StencilCmp::Never:return D3D12_COMPARISON_FUNC_NEVER;
	case StencilCmp::Less:return D3D12_COMPARISON_FUNC_LESS;
	case StencilCmp::Equal:return D3D12_COMPARISON_FUNC_EQUAL;
	case StencilCmp::LEqual:return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case StencilCmp::Greater:return D3D12_COMPARISON_FUNC_GREATER;
	case StencilCmp::NotEqual:return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case StencilCmp::GEqual:return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case StencilCmp::Always:return D3D12_COMPARISON_FUNC_ALWAYS;
	default:AssertEx(0, "%d", value); return D3D12_COMPARISON_FUNC_GREATER;
	}
}
inline D3D12_STENCIL_OP DX12GetStencilOp(StencilOp value)
{
	switch (value)
	{
	case StencilOp::Keep:return D3D12_STENCIL_OP_KEEP;
	case StencilOp::Zero:return D3D12_STENCIL_OP_ZERO;
	case StencilOp::Replace: return  D3D12_STENCIL_OP_REPLACE;
	case StencilOp::Incr: return D3D12_STENCIL_OP_INCR_SAT;
	case StencilOp::Incr_Wrap: return D3D12_STENCIL_OP_INCR;
	case StencilOp::Decr: return D3D12_STENCIL_OP_DECR_SAT;
	case StencilOp::Decr_Wrap: return D3D12_STENCIL_OP_DECR;
	default: AssertEx(0, "%d", value); return D3D12_STENCIL_OP_KEEP;
	}
}
inline D3D_PRIMITIVE_TOPOLOGY DX12GetPrimitiveType(PrimitiveType value)
{
	switch (value)
	{
	case PrimitiveType::PointList:return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PrimitiveType::LineList:return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveType::LineStrip:return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case PrimitiveType::TriangleList:return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveType::TriangleStrip:return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:AssertEx(0, "%d", value); return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

inline UINT DX12CalcConstantBufferByteSize(UINT byteSize)
{
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

class DX12Device;
extern DX12Device* GetDX12Device();

DC_END_NAMESPACE