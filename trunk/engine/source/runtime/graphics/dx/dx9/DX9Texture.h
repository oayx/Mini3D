
/*****************************************************************************
* Author： hannibal
* Date：2009年11月21日
* Description： 纹理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/Texture.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9Texture Final : public Texture
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9Texture);
	FRIEND_CONSTRUCT_DESTRUCT(DX9Texture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(DX9Texture);
	BEGIN_DERIVED_REFECTION_TYPE(DX9Texture, Texture)
	END_DERIVED_REFECTION_TYPE;

	DX9Texture(const TextureDesc& desc);
public:
	~DX9Texture();

public:
	virtual bool  Fill(Image* image)override;
	virtual byte* Lock(TextureLockDesc& lock_desc)override;
	virtual void  Unlock(const TextureLockDesc& lock_desc)override;

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override { return _colorTexture; }
	virtual void* GetTextureView()override { return _colorTexture; }

	virtual void SaveToFile(const String& name, ImageType type)override;

private:
	void CreateTexture();

protected:
	IDirect3DBaseTexture9* _colorTexture = nullptr;
};//DX9Texture
DC_END_NAMESPACE