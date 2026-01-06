#include "DX11RenderTexture.h"
#include "DX11Device.h"
#include "DX11RenderContent.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11RenderTexture, RenderTexture);
DX11RenderTexture::DX11RenderTexture(const TextureDesc& desc)
	: base(desc)
{
	if (_flags & TextureFlag::COLOR)
	{
		this->CreateRenderTarget();
	}
	if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		this->CreateDepthStencil();
	}
}
DX11RenderTexture::~DX11RenderTexture()
{
	SAFE_RELEASE(_colorTexture);
	SAFE_RELEASE(_colorSRV);
	for (int i = 0; i < 6; ++i)
		SAFE_RELEASE(_colorRTV[i]);
	SAFE_RELEASE(_depthTexture);
	SAFE_RELEASE(_depthSRV);
	SAFE_RELEASE(_depthDSV);
	SAFE_RELEASE(_stagingTexture);
}
void DX11RenderTexture::PreRender()
{
}
void DX11RenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	AssertEx(desc.face < (_textureType == TextureType::Cube ? 6 : 1), "");

	desc.viewPort.Set((float)_imageWidth, (float)_imageHeight);
	desc.targetBuffer = _colorRTV[desc.face];
	desc.depthStencilBuffer = _depthDSV;
	base::BeginFrame(desc);
}
void DX11RenderTexture::PostRender()
{
}
bool DX11RenderTexture::GetData(MemoryDataStream& stream)
{
	if (_textureType != TextureType::D2)
		return false;

	//把纹理拷贝到staging
	if (_stagingTexture == nullptr)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		_colorTexture->GetDesc(&desc);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateTexture2D(&desc, NULL, &_stagingTexture));
	}
	GetDX11Device()->GetContent()->CopyResource(_stagingTexture, _colorTexture);

	//获得数据
	D3D11_MAPPED_SUBRESOURCE mapped;
	DX_ERROR(GetDX11Device()->GetContent()->Map(_stagingTexture, 0, D3D11_MAP_READ, 0, &mapped));
	stream.Resize(mapped.RowPitch * _imageHeight);
	Memory::Copy(stream.Buffer(), mapped.pData, mapped.RowPitch * _imageHeight);
	GetDX11Device()->GetContent()->Unmap(_stagingTexture, 0);

	return true;
}
bool DX11RenderTexture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");

	GetDX11Device()->GetContent()->CopyResource((ID3D11Texture2D*)dst->GetTexture(), _colorTexture);
	return true;
}
void* DX11RenderTexture::GetTexture()
{
	if (_flags & TextureFlag::COLOR && (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL))
	{
		return _colorTexture;
	}
	else if (_flags & TextureFlag::COLOR)
	{
		return _colorTexture;
	}
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		return _depthTexture;
	}
	return _colorTexture;
}
void* DX11RenderTexture::GetTextureView()
{
	if (_flags & TextureFlag::COLOR && (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL))
	{
		return _colorSRV;
	}
	else if (_flags & TextureFlag::COLOR)
	{
		return _colorSRV;
	}
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		return _depthSRV;
	}
	return _colorSRV;
}
void DX11RenderTexture::CreateRenderTarget()
{
	{
		uint flags = 0;
		if (_textureType == TextureType::Cube)
			flags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		if (_enableMips)
			flags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = _imageWidth;
		tex_desc.Height = _imageHeight;
		tex_desc.Format = DX10GetTextureFormat(_imageFormat, false);
		tex_desc.MipLevels = _enableMips ? 9 : 1;
		tex_desc.ArraySize = _textureType == TextureType::Cube ? 6 : 1;
		tex_desc.SampleDesc.Count = _antiAlias;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = 0;
		tex_desc.MiscFlags = flags;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateTexture2D(&tex_desc, NULL, &_colorTexture));
	}
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = DX10GetShaderViewFormat(_imageFormat, false);
		if (_textureType == TextureType::Cube)
			view_desc.ViewDimension = _antiAlias > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURECUBE;
		else
			view_desc.ViewDimension = _antiAlias > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = 1;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateShaderResourceView(_colorTexture, &view_desc, &_colorSRV));
	}
	{
		D3D11_RENDER_TARGET_VIEW_DESC rt_desc = {};
		rt_desc.Format = DX10GetTextureViewFormat(_imageFormat, false);
		if (_textureType == TextureType::Cube)
			rt_desc.ViewDimension = _antiAlias > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		else
			rt_desc.ViewDimension = _antiAlias > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		rt_desc.Texture2DArray.MipSlice = 0;
		if (_textureType == TextureType::Cube)
		{
			for (int i = 0; i < 6; ++i)
			{
				rt_desc.Texture2DArray.ArraySize = 1;
				rt_desc.Texture2DArray.FirstArraySlice = i;
				DX_ERROR(GetDX11Device()->GetDevice()->CreateRenderTargetView(_colorTexture, &rt_desc, &_colorRTV[i]));
			}
		}
		else
		{
			DX_ERROR(GetDX11Device()->GetDevice()->CreateRenderTargetView(_colorTexture, &rt_desc, &_colorRTV[0]));
		}
	}
	//mipmap
	if (_enableMips)
	{
		GetDX11Device()->GetContent()->GenerateMips(_colorSRV);
	}
}
void DX11RenderTexture::CreateDepthStencil()
{
	{
		D3D11_TEXTURE2D_DESC depth_desc = {};
		depth_desc.Width = _imageWidth;
		depth_desc.Height = _imageHeight;
		depth_desc.Format = DX10GetDepthTextureFormat(_imageFormat);
		depth_desc.MipLevels = 1;
		depth_desc.ArraySize = 1;
		depth_desc.SampleDesc.Count = _antiAlias;
		depth_desc.SampleDesc.Quality = 0;
		depth_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_desc.CPUAccessFlags = 0;
		depth_desc.MiscFlags = 0;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateTexture2D(&depth_desc, NULL, &_depthTexture));
	}
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Format = DX10GetDepthShaderViewFormat(_imageFormat);
		view_desc.ViewDimension = _antiAlias > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = 1;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateShaderResourceView(_depthTexture, &view_desc, &_depthSRV));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depth_view_desc = {};
		depth_view_desc.Format = DX10GetDepthTextureViewFormat(_imageFormat);
		depth_view_desc.Flags = 0;
		depth_view_desc.ViewDimension = _antiAlias > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_view_desc.Texture2D.MipSlice = 0;
		DX_ERROR(GetDX11Device()->GetDevice()->CreateDepthStencilView(_depthTexture, &depth_view_desc, &_depthDSV));
	}
}
void DX11RenderTexture::SaveToFile(const String& name, ImageType type)
{
	Image* image = Image::Create(_imageFormat, iSize(_imageWidth, _imageHeight), true);
	MemoryDataStream stream;
	if (this->GetData(stream) && image->GetSize() == stream.Size())
	{
		image->Fill(stream.Buffer(), stream.Size());
		image->SaveToFile(name);
	}
}
DC_END_NAMESPACE