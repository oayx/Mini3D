 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/21
* Description： 纹理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/Texture.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
class TextureUnit;
/********************************************************************/
class DX12Texture final : public Texture
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12Texture);
	FRIEND_CONSTRUCT_DESTRUCT(DX12Texture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(DX12Texture);
	BEGIN_DERIVED_REFECTION_TYPE(DX12Texture, Texture)
	END_REFECTION_TYPE;

protected:
	DX12Texture(const TextureDesc& desc);
public:
	~DX12Texture();

public:
	virtual bool  Fill(Image* image)override;
	virtual byte* Lock(TextureLockDesc& lock_desc)override;
	virtual void  Unlock(const TextureLockDesc& lock_desc)override;

	virtual bool  Copy(Texture* dst)override { return false; }
	virtual void* GetTexture()override { return _colorTexture; }
	virtual void* GetTextureView()override { return _colorTextureView; }

	virtual void  SaveToFile(const String& name, ImageType type)override {}

private:
	void CreateDescriptorHeaps();
	void CreateTexture();
	void CreateTextureView();

protected:
	ID3D12Resource*			_colorTexture = nullptr;
	ID3D12DescriptorHeap*	_colorTextureView = nullptr;
};//DX12Texture
DC_END_NAMESPACE

