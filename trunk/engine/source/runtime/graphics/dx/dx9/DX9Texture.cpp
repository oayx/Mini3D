#include "DX9Texture.h"
#include "DX9Device.h"
#include "DX9Caps.h"
#include "core/Utility.h"
#include "core/image/Image.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9Texture, Texture);
DX9Texture::DX9Texture(const TextureDesc& desc)
	: base(desc)
{
	AssertEx(_textureType == TextureType::D2 || _textureType == TextureType::Cube, "");

	if (this->IsStaging())
	{
		_enableMips = false;
		AssertEx(_textureType != TextureType::Cube, "");
	}

	if (_enableMips)
	{
		switch (_textureType)
		{
		case TextureType::D2:
			_enableMips = GetGraphicsCaps()->hasMipTexture;
			break;
		case TextureType::Cube:
			_enableMips = GetGraphicsCaps()->hasCubeMipTexture;
			break;
		}
	}

	this->CreateTexture();
}
DX9Texture::~DX9Texture()
{
	SAFE_RELEASE(_colorTexture);
}
bool DX9Texture::Fill(Image* image)
{
	for (int level = 0; level < image->MipLevels(); ++level)
	{
		ImageMipData* mip_data = image->GetMipData(level);
		if (level == 0)this->_totalBytes = mip_data->Size;

		TextureLockDesc lock_desc(level);
		void *pdata = this->Lock(lock_desc);
		Memory::Copy(pdata, mip_data->Data, mip_data->Size);
		this->Unlock(lock_desc);

		if (_textureType == TextureType::D2 && !_enableMips)break;
	}
	
	return true;
}
byte* DX9Texture::Lock(TextureLockDesc& lock_desc)
{
	if (lock_desc.flags == GPUResourceLock::NoOverWrite)
	{
		AssertEx(_textureType == TextureType::D2 && lock_desc.level == 0, "");

		lock_desc.pitch = this->GetPitch();
		if (_imageData == nullptr)
		{
			uint size = this->GetBytes();
			_imageData = Memory::NewArray<byte>(size);
			Memory::Clear(_imageData, size);
		}
		return _imageData;
	}
	else
	{
		DWORD lock_type = D3DLOCK_DISCARD;
		if (lock_desc.flags == GPUResourceLock::ReadOnly) lock_type = D3DLOCK_READONLY;
		switch (_textureType)
		{
		case TextureType::D2:
		{
			IDirect3DTexture9* texture = (IDirect3DTexture9*)_colorTexture;
			D3DLOCKED_RECT rect;
			DX9HR(texture->LockRect(lock_desc.level, &rect, 0, 0));
			lock_desc.pitch = rect.Pitch;
			return (byte*)rect.pBits;
		}
		case TextureType::Cube:
		{//立方体锁定时，level表示face
			IDirect3DCubeTexture9* texture = (IDirect3DCubeTexture9*)_colorTexture;
			D3DLOCKED_RECT rect;
			DX9HR(texture->LockRect(D3DCUBEMAP_FACES(lock_desc.level), 0, &rect, nullptr, lock_type));
			lock_desc.pitch = rect.Pitch;
			return (byte*)rect.pBits;
		}
		default:
			return nullptr;
		}
	}
}
void  DX9Texture::Unlock(const TextureLockDesc& lock_desc)
{
	if (lock_desc.flags == GPUResourceLock::NoOverWrite)
	{
		AssertEx(_textureType == TextureType::D2 && lock_desc.level == 0, "");

		IDirect3DTexture9* texture = (IDirect3DTexture9*)_colorTexture;
		D3DLOCKED_RECT rect;
		DX9HR(texture->LockRect(0, &rect, 0, 0));
		Memory::Copy(rect.pBits, _imageData, this->GetBytes());
		DX9HR(texture->UnlockRect(0));
	}
	else
	{
		switch (_textureType)
		{
		case TextureType::D2:
		{
			IDirect3DTexture9* texture = (IDirect3DTexture9*)_colorTexture;
			DX9HR(texture->UnlockRect(lock_desc.level));
			break;
		}
		case TextureType::Cube:
		{
			IDirect3DCubeTexture9* texture = (IDirect3DCubeTexture9*)_colorTexture;
			DX9HR(texture->UnlockRect(D3DCUBEMAP_FACES(lock_desc.level), 0));
			break;
		}
		}
	}
}
bool DX9Texture::GetData(MemoryDataStream& stream)
{
	if (_textureType != TextureType::D2)
		return false;

	TextureLockDesc lock_desc(0, GPUResourceLock::ReadOnly);
	byte* s_bits = this->Lock(lock_desc);
	if (s_bits)
	{
		stream.Resize(lock_desc.pitch * _imageHeight);
		Memory::Copy(stream.Buffer(), s_bits, lock_desc.pitch * _imageHeight);
	}
	this->Unlock(lock_desc);
	return true;
}
bool DX9Texture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");

	TextureLockDesc lock_desc(0);
	void* d_bits = dst->Lock(lock_desc);
	{
		void* s_bits = this->Lock(lock_desc);
		Memory::Copy(d_bits, s_bits, this->GetBytes());
		this->Unlock(lock_desc);
	}
	dst->Unlock(lock_desc);
	return true;
}
void DX9Texture::SaveToFile(const String& name, ImageType type)
{
	std::wstring wPath = Encoding::Utf8ToWChar(name.c_str(), name.Size());
	HRESULT hr = D3DXSaveTextureToFileW(wPath.c_str(), DX9GetImageType(type), _colorTexture, NULL);
	if (FAILED(hr))
	{
		Debuger::Error("DX9Texture::SaveToFile on D3DXSaveTextureToFile 失败(%s)", name.c_str());
	}
}
void DX9Texture::CreateTexture()
{
	DWORD usage = 0;
	D3DFORMAT format = DX9GetTextureFormat(_imageFormat);
	switch (_textureType)
	{
		case TextureType::D2:
		{
			IDirect3DTexture9* texture = nullptr;
			DX9HR(GetDX9Device()->GetDevice()->CreateTexture(
				_imageWidth,
				_imageHeight,
				_enableMips ? 0 : 1,
				usage,
				format,
				D3DPOOL_MANAGED,
				&texture,
				NULL));
			_colorTexture = texture;
		}
		break;
		case TextureType::Cube:
		{
			if (!GetGraphicsCaps()->hasCubeTexture)
				return;
			IDirect3DCubeTexture9 * cube_texture = nullptr;
			DX9HR(GetDX9Device()->GetDevice()->CreateCubeTexture(
				_imageWidth,
				_enableMips ? 0 : 1,
				usage,
				format,
				D3DPOOL_MANAGED,
				&cube_texture,
				NULL));
			_colorTexture = cube_texture;
		}
		break;
	}
}
DC_END_NAMESPACE