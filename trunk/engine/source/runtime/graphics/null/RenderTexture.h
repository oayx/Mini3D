 
/*****************************************************************************
* Author： hannibal
* Date：2010/4/15
* Description： 
*****************************************************************************/
#pragma once

#include "Texture.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Texture;
class Renderer;
/********************************************************************/
#pragma pack(push,1)
class RenderTargetKey
{
public:
	union
	{
		struct
		{
			int width : 16;
			int height : 16;
			ColorFormat color_format : 6;
			ColorFormat depth_format : 6;
			TextureFilter filter_mode : 2;
			TextureAddress wrap_mode : 2;
			TextureFlag flags : 16;
		};
		uint64 u = 0;
	};
};
#pragma pack(pop)

// 渲染到纹理
class ENGINE_DLL RenderTexture : public Texture
{
	typedef Map<uint64, List<RenderTexture*>> RenderTextures;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(RenderTexture);
	BEGIN_DERIVED_REFECTION_TYPE(RenderTexture, Texture)
	END_DERIVED_REFECTION_TYPE;

protected:
	RenderTexture(const TextureDesc& desc)
		: base(desc) {}

public:
	static RenderTexture* Alloc(const TextureDesc& desc);
	static void Free(RenderTexture* rt);
	static void Free();

public:
	virtual byte* Lock(TextureLockDesc& lock_desc)override { AssertEx(false, "cannot lock render texture"); return nullptr; }
	virtual void  Unlock(const TextureLockDesc& lock_desc)override { AssertEx(false, "cannot unlock render texture"); }
	virtual void PreRender() = 0;
	virtual void BeginFrame(RenderFrameDesc& desc);
	virtual void EndFrame();
	virtual void PostRender() = 0;
	virtual void Resize(const WindowResizeDesc& desc) = 0;

protected:
	RenderTargetKey _key;

private:
	static RenderTextures _renderTextures;
};
DC_END_NAMESPACE
