#include "DX9RenderTexture.h"
#include "DX9Device.h"
#include "DX9Caps.h"
#include "DX9RenderContent.h"
#include "core/Utility.h"
#include "runtime/Application.h"
#include "runtime/Screen.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9RenderTexture, RenderTexture);
DX9RenderTexture::DX9RenderTexture(const TextureDesc& desc)
: base(desc)
{
	this->CreateRenderTarget();
	if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		this->CreateDepthStencil();
	}
}
DX9RenderTexture::~DX9RenderTexture()
{
	SAFE_RELEASE(_colorTexture);
	SAFE_RELEASE(_depthStencilSurface);
	for (int i = 0; i < 6; ++i)SAFE_RELEASE(_colorSurface[i]);
}
void DX9RenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	AssertEx(desc.face < (_textureType == TextureType::Cube ? 6 : 1), "");

	if (_flags & TextureFlag::STENCIL)
		desc.clearFlag |= ClearFlag::Stencil;
	else
		desc.clearFlag ^= ClearFlag::Stencil;

	desc.viewPort.Set((float)_imageWidth, (float)_imageHeight);
	desc.targetBuffer = _colorSurface[desc.face];
	desc.depthStencilBuffer = _depthStencilSurface;
	base::BeginFrame(desc);
}
bool DX9RenderTexture::GetData(MemoryDataStream& stream)
{
	if (_textureType != TextureType::D2)
		return false;

	HRESULT hr = E_FAIL;
	IDirect3DSurface9* backbuffer = nullptr;
	IDirect3DSurface9* surface = nullptr;
	do
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)_colorTexture;
		hr = texture->GetSurfaceLevel(0, &backbuffer);
		if (FAILED(hr))break;

		D3DSURFACE_DESC rt_desc;
		hr = backbuffer->GetDesc(&rt_desc);
		if (FAILED(hr))break;

		hr = GetDX9Device()->GetDevice()->CreateOffscreenPlainSurface(rt_desc.Width, rt_desc.Height, rt_desc.Format, D3DPOOL_SYSTEMMEM, &surface, NULL);
		if (FAILED(hr))break;
		hr = GetDX9Device()->GetDevice()->GetRenderTargetData(backbuffer, surface);
		if (FAILED(hr))break;

		D3DLOCKED_RECT lock_desc;
		hr = surface->LockRect(&lock_desc, nullptr, D3DLOCK_READONLY);
		if (FAILED(hr))break;
		{//复制数据
			byte* s_bits = (byte*)lock_desc.pBits;
			stream.Resize(lock_desc.Pitch * _imageHeight);
			Memory::Copy(stream.Buffer(), s_bits, lock_desc.Pitch * _imageHeight);
		}
		surface->UnlockRect();
	} while (false);

	SAFE_RELEASE(backbuffer);
	SAFE_RELEASE(surface);

	return true;
}
bool DX9RenderTexture::Copy(Texture* dst)
{
	CHECK_RETURN_PTR_FALSE(_colorTexture && dst);
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");
	if (_textureType != TextureType::D2)
		return false;

	TextureLockDesc lock_desc;
	byte* bits = dst->Lock(lock_desc);
	if (bits)
	{
		MemoryDataStream stream;
		if (this->GetData(stream))
		{
			Memory::Copy(bits, stream.Buffer(), stream.Size());
		}
	}
	dst->Unlock(lock_desc);

	return true;
}
void DX9RenderTexture::SaveToFile(const String& name, ImageType format)
{
	std::wstring wPath = Encoding::Utf8ToWChar(name.c_str(), name.Size());
	HRESULT hr = D3DXSaveTextureToFileW(wPath.c_str(), DX9GetImageType(format), _colorTexture, NULL);
	if (FAILED(hr))
	{
		Debuger::Error("D3DXSaveTextureToFile 失败(%s)", name.c_str());
	}
	else
	{
		Debuger::Log("D3DXSaveTextureToFile file(%s)", name.c_str());
	}
}
void DX9RenderTexture::CreateRenderTarget()
{
	uint usage = D3DUSAGE_RENDERTARGET;
	switch (_textureType)
	{
	case TextureType::D2:
	{
		IDirect3DTexture9* texture = nullptr;
		DX9HR(GetDX9Device()->GetDevice()->CreateTexture(
			_imageWidth,
			_imageHeight,
			1,
			usage,
			DX9GetRenderTargetFormat(_imageFormat),
			D3DPOOL_DEFAULT,
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
			1,
			usage,
			DX9GetRenderTargetFormat(_imageFormat),
			D3DPOOL_DEFAULT,
			&cube_texture,
			NULL));
		_colorTexture = cube_texture;
	}
	break;
	}

	if (_textureType == TextureType::D2)
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)_colorTexture;
		DX9HR(texture->GetSurfaceLevel(0, &_colorSurface[0]));
	}
	else if (_textureType == TextureType::Cube)
	{
		IDirect3DCubeTexture9* texture = (IDirect3DCubeTexture9*)_colorTexture;
		for (int i = 0; i < 6; ++i)
		{
			DX9HR(texture->GetCubeMapSurface(D3DCUBEMAP_FACES(D3DCUBEMAP_FACE_POSITIVE_X + i), 0, &_colorSurface[i]));
		}
	}
}
void DX9RenderTexture::CreateDepthStencil()
{
	D3DMULTISAMPLE_TYPE msaa = D3DMULTISAMPLE_TYPE::D3DMULTISAMPLE_NONE;
	if (_flags & TextureFlag::MSAA)
	{
		msaa = (D3DMULTISAMPLE_TYPE)DX9Caps::GetMSAAQualityLevel(4, D3DFMT_D24S8, !Screen::IsFullscreen()).Count;
	}
	DX9HR(GetDX9Device()->GetDevice()->CreateDepthStencilSurface(
		_imageWidth, 
		_imageHeight, 
		D3DFMT_D24S8,
		msaa, 
		0, 
		TRUE, 
		&_depthStencilSurface, 
		NULL));
}
DC_END_NAMESPACE