 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/21
* Description： 纹理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/Texture.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX11Texture Final : public Texture
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11Texture);
	FRIEND_CONSTRUCT_DESTRUCT(DX11Texture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(DX11Texture);
	BEGIN_DERIVED_REFECTION_TYPE(DX11Texture, Texture)
	END_DERIVED_REFECTION_TYPE;

protected:
	DX11Texture(const TextureDesc& desc);

public:
	~DX11Texture();
	virtual bool  Fill(Image* image)override;
	virtual byte* Lock(TextureLockDesc& lock_desc)override;
	virtual void  Unlock(const TextureLockDesc& lock_desc)override;

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override { return _colorTexture; }
	virtual void* GetTextureView()override { return _colorTextureView; }

	virtual void SaveToFile(const String& name, ImageType type)override;

private:
	void CreateTexture();
	void CreateTextureView();

protected:
	ID3D11Texture2D*			_colorTexture = nullptr;
	ID3D11Texture2D*			_stagingTexture = nullptr;
	ID3D11Texture2D*			_tempStagingTexture = nullptr;
	ID3D11ShaderResourceView*	_colorTextureView = nullptr;
};//DX11Texture
DC_END_NAMESPACE
