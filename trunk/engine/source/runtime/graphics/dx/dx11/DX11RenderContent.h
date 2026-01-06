 
/*****************************************************************************
* Author： hannibal
* Date：2020/2/20
* Description：d3d渲染
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/RenderContent.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
#pragma pack(push,1)
struct RasterizerStateKey
{
	union {
		struct {
			CullMode CullMode : 8;
			FillMode FillMode : 8;
			bool DepthClipEnable : 1;
			bool ScissorEnable : 1;
			uint padding : 14;
		};
		uint u = 0;
	};
};
struct BlendStateKey
{
	union {
		struct {
			bool enable_alpha : 1;
			bool alpha_to_coverage : 1;
			AlphaBlend src_alpha_blend : 6;
			AlphaBlend dst_alpha_blend : 6;
			AlphaBlend src_color_blend : 6;
			AlphaBlend dst_color_blend : 6;
			bool color_mask_r : 1;
			bool color_mask_g : 1;
			bool color_mask_b : 1;
			bool color_mask_a : 1;
			uint padding : 2;
		};
		uint u = 0;
	};
};
struct DepthStencilStateKey
{
	union {
		struct {
			bool DepthEnable : 1;
			bool DepthWriteEnable : 1;
			StencilCmp DepthCmpFun : 6;
			bool StencilEnable : 1;
			byte StencilReadMask : 8;
			byte StencilWriteMask : 8;
			StencilOp StencilFailOp : 8;
			StencilOp StencilDepthFailOp : 8;
			StencilOp StencilPassOp : 8;
			StencilCmp StencilFunc : 6;
			byte StencilRef : 8;
			byte padding : 1;
		};
		uint64 u = 0;
	};
};
struct SamplerStateKey
{
	union {
		struct {
			TextureAddress AddressMode : 8;
			uint BorderColor : 32;
			TextureFilter FilterType : 8;
			byte Anisotropy : 8;
			byte padding : 8;
		};
		uint64 u;
	};
};
struct VertexDeclarationKey
{
	union {
		struct {
			uint File : 32;
			byte Position : 1;
			byte Diffuse : 1;
			byte Normal : 1;
			byte Tangent : 1;
			byte Coordinates1 : 1;
			byte Coordinates2 : 1;
			byte Weights : 1;
			byte Indices : 1;
			byte Size : 1;
			byte InstanceOffset : 1;
			byte Stream : 8;
			ushort padding : 14;
		};
		uint64 u;
	};
};
struct StagingTextureKey 
{
	union {
		struct {
			uint Width : 23;
			uint Height : 23;
			ColorFormat Format : 8;
			byte MipLevels : 4;
			byte ArraySize : 4;
			bool IsReadOnly : 1;
			byte padding : 1;
		};
		uint64 u;
	};
};
#pragma pack(pop)
/********************************************************************/
class DX11RenderContent final : public RenderContent
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11RenderContent);
	FRIEND_CONSTRUCT_DESTRUCT(DX11RenderContent);
	DISALLOW_COPY_ASSIGN(DX11RenderContent);
	BEGIN_DERIVED_REFECTION_TYPE(DX11RenderContent, RenderContent)
	END_REFECTION_TYPE;

	DX11RenderContent();
	~DX11RenderContent();

public:
	virtual void Initialize()override;

	virtual void PreRender(RenderWindow* window)override { base::PreRender(window); }
	virtual void PostRender()override { base::PostRender(); }
	virtual void BeginFrame(RenderFrameDesc& desc)override;
	virtual void RenderOnePrimitive(Camera* camera, Pass* pass, Primitive* primitive, RenderMode mode)override;
	virtual void Present(uint sync)override {}

	virtual void SetViewport(const ViewPortDesc& viewPort)override;
	virtual void SetViewportScissor(const iRect& clip)override;
	virtual void ClearBackbuffer(void* targetBuffer, void* depthStencilBuffer, ClearFlag flag, const Color& color)override;

	virtual void Resize(const WindowResizeDesc& desc)override;

public:
	ID3D11SamplerState* GetSamplerState(SamplerStateKey state);

private:
	void UpdataRenderState(Camera* camera, Pass* pass);
	void SetRasterizerState(Camera* camera, Pass* pass);
	void SetBlendState(Pass* pass);
	void SetDepthStencilTest(Pass* pass);
	void SetSubPrimitiveState(const SubPrimitive* subPrim, Pass* pass);
	void SetVertexDeclaration(CGProgram* shader, HardwareVertexBuffer* vertex_buffer);

private:
	std::unordered_map<uint, ID3D11RasterizerState*> _rasterizerStates;
	std::unordered_map<uint64, ID3D11DepthStencilState*> _depthStencilStates;
	std::unordered_map<uint, ID3D11BlendState*> _blendStates;
	std::unordered_map<uint, ID3D11SamplerState*> _samplerStates;
};
DC_END_NAMESPACE
