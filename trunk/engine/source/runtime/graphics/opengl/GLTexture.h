 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/21
* Description： 纹理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/Texture.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLTexture final : public Texture
{
	friend class GLDevice;
	DEFAULT_CREATE(GLTexture);
	FRIEND_CONSTRUCT_DESTRUCT(GLTexture);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(GLTexture);
	BEGIN_DERIVED_REFECTION_TYPE(GLTexture, Texture)
	END_REFECTION_TYPE;

protected:
	GLTexture(const TextureDesc& desc);
	~GLTexture();

public:
	virtual bool  Fill(Image* image)override;
	virtual byte* Lock(TextureLockDesc& lock_desc)override;
	virtual void  Unlock(const TextureLockDesc& lock_desc)override;

	virtual bool  GetData(MemoryDataStream& stream)override;
	virtual bool  Copy(Texture* dst)override;
	virtual void* GetTexture()override { return &_colorTexture; }
	virtual void* GetTextureView()override { return &_colorTexture; }

	virtual void SaveToFile(const String& name, ImageType type)override;

private:
	bool UploadTexture(int w, int h, ImageMipData* image, int idx);

protected:
	GLuint	_colorTexture = 0;
};//GLTexture
DC_END_NAMESPACE
