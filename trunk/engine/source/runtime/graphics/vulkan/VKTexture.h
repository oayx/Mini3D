 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/21
* Description： 纹理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/Texture.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class VKTexture final : public Texture
{
	friend class VKDevice;
	DEFAULT_CREATE(VKTexture);
	FRIEND_CONSTRUCT_DESTRUCT(VKTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(VKTexture);
	BEGIN_DERIVED_REFECTION_TYPE(VKTexture, Texture)
	END_REFECTION_TYPE;

protected:
	VKTexture(const TextureDesc& desc);
public:
	~VKTexture();

public:
	virtual bool  Fill(Image* image)override;
	virtual byte* Lock(TextureLockDesc& lock_desc)override;
	virtual void  Unlock(const TextureLockDesc& lock_desc)override;

	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override { return (void*)_colorTexture; }
	virtual void* GetTextureView()override { return (void*)_colorTextureView; }
	virtual void* GetTextureSample()override { return (void*)_textureSampler; }

	virtual void SaveToFile(const String& name, ImageType type)override;

private:
	void CreateTexture();
	void CreateTextureView();
	void CreateTextureSampler();

protected:
	VkImage			_colorTexture = VK_NULL_HANDLE;
	VkImageView		_colorTextureView = VK_NULL_HANDLE;
	VkDeviceMemory	_textureMemory = VK_NULL_HANDLE;
	VkSampler		_textureSampler = VK_NULL_HANDLE;
};//VKTexture
DC_END_NAMESPACE
