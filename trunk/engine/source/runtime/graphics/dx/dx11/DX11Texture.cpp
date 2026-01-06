#include "DX11Texture.h"
#include "DX11Device.h" 
#include "DX11RenderContent.h"
#include "core/image/Image.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11Texture, Texture);
DX11Texture::DX11Texture(const TextureDesc& desc)
	: base(desc)
{
	AssertEx(_textureType == TextureType::D2 || _textureType == TextureType::Cube, "");
	if (_enableMips)
	{
		_enableMips = GetDX11Device()->GetCaps()->IsGenerateMipsSupported(_textureType, _imageFormat);
	}
	if (this->IsStaging())
	{
		_enableMips = false;
		AssertEx(_textureType != TextureType::Cube, "");
	}
	
	CreateTexture();
	if (!this->IsStaging())
	{
		this->CreateTextureView();
	}
}
DX11Texture::~DX11Texture()
{
	SAFE_RELEASE(_colorTexture);
	SAFE_RELEASE(_colorTextureView);
	SAFE_RELEASE(_stagingTexture);
}
bool DX11Texture::Fill(Image* image)
{
	for (int level = 0; level < image->MipLevels(); ++level)
	{
		ImageMipData* mip_data = image->GetMipData(level);
		if (level == 0)this->_totalBytes = mip_data->Size;
		GetDX11Device()->GetContent()->UpdateSubresource(_colorTexture, D3D11CalcSubresource(level, 0, image->MipLevels()), NULL, mip_data->Data, mip_data->RowPitch, 0);

		if (_textureType == TextureType::D2 && !_enableMips)break;
	}

	return true;
}
void DX11Texture::CreateTexture()
{
	UINT bind_flags = 0;
	UINT misc_flags = 0;
	UINT cpu_flags = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (this->IsStaging())
	{
		usage = D3D11_USAGE_STAGING;
		cpu_flags = D3D11_CPU_ACCESS_READ;//TODO:也有可能是写
	}
	else
	{
		bind_flags |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (_enableMips)
	{
		bind_flags |= D3D11_BIND_RENDER_TARGET;//GenerateMips需要此标记，来自MSDN
		misc_flags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	if (_textureType == TextureType::Cube)
	{
		misc_flags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = _imageWidth;
	desc.Height = _imageHeight;
	desc.MipLevels = _enableMips ? 0 : 1;	//0自动生成全部，1只生成最大的一张，其他值生成对应数量的
	desc.ArraySize = _textureType == TextureType::Cube ? 6 : 1;//可以用于创建纹理数组，这里指定纹理的数目，单个纹理使用1
	desc.Format = DX10GetTextureFormat(_imageFormat, this->IsSRGB());
	desc.SampleDesc.Count = 1;			// MSAA采样数(对于经常作为着色器资源的纹理，通常是不能对其开启MSAA的，应当把Count设为1，Quality设为0)
	desc.SampleDesc.Quality = 0;		// MSAA质量等级
	desc.Usage = usage;
	desc.BindFlags = bind_flags;
	desc.CPUAccessFlags = cpu_flags;
	desc.MiscFlags = misc_flags;
	DX_ERROR(GetDX11Device()->GetDevice()->CreateTexture2D(&desc, NULL, &_colorTexture));
}
void DX11Texture::CreateTextureView()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
	view_desc.Format = DX10GetShaderViewFormat(_imageFormat, this->IsSRGB());
	if (_textureType == TextureType::Cube)
	{
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		view_desc.Texture2D.MostDetailedMip = 0;
		view_desc.Texture2D.MipLevels = _enableMips ? 0 : 1;
	}
	else if (_textureType == TextureType::D2)
	{
		view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		if (_enableMips)
		{
			view_desc.Texture2D.MostDetailedMip = 0;//指定开始使用的纹理子资源，如果要使用完整mipmaps，则需要指定MostDetailedMap为0， MipLevels为-1
			view_desc.Texture2D.MipLevels = -1;		//指定使用的子资源数目
		}
		else
		{
			view_desc.Texture2D.MostDetailedMip = 0;
			view_desc.Texture2D.MipLevels = 1;
		}
	}
	DX_ERROR(GetDX11Device()->GetDevice()->CreateShaderResourceView(_colorTexture, &view_desc, &_colorTextureView));
}
byte* DX11Texture::Lock(TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2)
		return nullptr;

	if (!this->IsStaging())
	{
		if (_stagingTexture == nullptr)
		{//先把GUP数据拷贝到staging
			D3D11_TEXTURE2D_DESC desc = {};
			_colorTexture->GetDesc(&desc);
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = lock_desc.flags == GPUResourceLock::ReadOnly ? D3D11_CPU_ACCESS_READ : D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			DX_ERROR(GetDX11Device()->GetDevice()->CreateTexture2D(&desc, NULL, &_stagingTexture));
		}
		_tempStagingTexture = _stagingTexture;
	}
	else
	{
		_tempStagingTexture = _colorTexture;
	}

	D3D11_MAP map_type = D3D11_MAP_WRITE;
	if (lock_desc.flags == GPUResourceLock::ReadOnly)map_type = D3D11_MAP_READ;

	D3D11_MAPPED_SUBRESOURCE mapped;
	DX_ERROR(GetDX11Device()->GetContent()->Map(_tempStagingTexture, lock_desc.level, map_type, 0, &mapped));
	lock_desc.pitch = mapped.RowPitch;
	return (byte*)mapped.pData;
}
void  DX11Texture::Unlock(const TextureLockDesc& lock_desc)
{
	if (_textureType != TextureType::D2)
		return;

	AssertEx(_tempStagingTexture != nullptr, "");
	GetDX11Device()->GetContent()->Unmap(_tempStagingTexture, lock_desc.level);
	if (lock_desc.flags != GPUResourceLock::ReadOnly && _tempStagingTexture != _colorTexture)
	{//提交纹理
		GetDX11Device()->GetContent()->CopyResource(_colorTexture, _tempStagingTexture);
	}
	_tempStagingTexture = nullptr;
}
bool DX11Texture::GetData(MemoryDataStream& stream)
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
bool DX11Texture::Copy(Texture* dst)
{
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");
	//D3D11_BOX srcBox;
	//srcBox.left = (UINT)offset.x;
	//srcBox.top = (UINT)offset.y;
	//srcBox.front = 0;
	//srcBox.right = (UINT)(offset.x + size.width);
	//srcBox.bottom = (UINT)(offset.y + size.height);
	//srcBox.back = 1;
	//GetDX11Device()->GetContent()->CopySubresourceRegion((ID3D11Resource*)dst->GetTexture(), 0, 0, 0, 0, _colorTexture, 0, &srcBox);

	CHECK_RETURN_PTR_FALSE(dst->GetWidth() == _imageWidth && dst->GetHeight() == _imageHeight && dst->GetFormat() == _imageFormat);
	GetDX11Device()->GetContent()->CopyResource((ID3D11Resource*)dst->GetTexture(), _colorTexture);
	return true;
}
void DX11Texture::SaveToFile(const String& name, ImageType type)
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

/*MipLevels
如果你希望它不产生mipmap，则应当指定为1(只包含最大的位图本身)
如果你希望它能够产生完整的mipmap，可以指定为0，这样你就不需要手工去算这个纹理最大支持的mipmap等级数了，在创建好纹理后，可以再调用ID3D11Texture2D::GetDesc来查看实际的MipLevels值是多少
如果你指定的是其它的值，这里举个例子，该纹理的宽高为400x400，mip等级为3时，该纹理会产生400x400，200x200和100x100的mipmap
*/

/*Usage：
D3D11_USAGE			CPU读	CPU写	GPU读	GPU写
D3D11_USAGE_DEFAULT					√		√
D3D11_USAGE_IMMUTABLE				√
D3D11_USAGE_DYNAMIC			√		√
D3D11_USAGE_STAGING	√		√		√		√

如果一个纹理以D3D11_USAGE_DEFAULT的方式创建，那么它可以使用下面的这些方法来更新纹理：
	ID3D11DeviceContext::UpdateSubresource
	ID3D11DeviceContext::CopyResource
	ID3D11DeviceContext::CopySubresourceRegion

而如果一个纹理以D3D11_USAGE_IMMUTABLE的方式创建，则必须在创建阶段就完成纹理资源的初始化。此后GPU只能读取，也无法对纹理再进行修改

D3D11_USAGE_DYNAMIC创建的纹理通常需要频繁从CPU写入，使用ID3D11DeviceContext::Map方法将显存映射回内存，经过修改后再调用ID3D11DeviceContext::Unmap方法应用更改(纹理只能是单个，不能是数组，且mip等级只能是1，即不能有mipmaps)

D3D11_USAGE_STAGING则完全允许在CPU和GPU之间的数据传输，但它只能作为一个类似中转站的资源，而不能绑定到渲染管线上，即你也不能用该纹理生成mipmaps
*/

/*D3D11_BIND_FLAG
D3D11_BIND_FLAG						描述
D3D11_BIND_SHADER_RESOURCE			纹理可以作为着色器资源绑定到渲染管线
D3D11_BIND_STREAM_OUTPUT			纹理可以作为流输出阶段的输出点
D3D11_BIND_RENDER_TARGET			纹理可以作为渲染目标的输出点，并且指定它可以用于生成mipmaps
D3D11_BIND_DEPTH_STENCIL			纹理可以作为深度/模板缓冲区
D3D11_BIND_UNORDERED_ACCESS			纹理可以绑定到无序访问视图作为输出
*/

/*CPUAccessFlags
D3D11_CPU_ACCESS_FLAG				描述
D3D11_CPU_ACCESS_WRITE				允许通过映射方式从CPU写入，它不能作为管线的输出，且只能用于D3D11_USAGE_DYNAMIC和D3D11_USAGE_STAGING绑定的资源
D3D11_CPU_ACCESS_READ				允许通过映射方式给CPU读取，它不能作为管线的输入或输出，且只能用于D3D11_USAGE_STAGING绑定的资源
可以用按位或的方式同时指定上述枚举值，如果该flag设为0可以获得更好的资源优化操作。
*/

/*MiscFlags
D3D11_RESOURCE_MISC_FLAG			描述
D3D11_RESOURCE_MISC_GENERATE_MIPS	允许通过ID3D11DeviceContext::GenerateMips方法生成mipmaps
D3D11_RESOURCE_MISC_TEXTURECUBE		允许该纹理作为纹理立方体使用，要求必须是至少包含6个纹理的Texture2DArray
*/

/*
D3D11_MAP枚举值类型的成员如下：
D3D11_MAP成员						描述
D3D11_MAP_READ						映射到内存的资源用于读取。该资源在创建的时候必须绑定了D3D11_CPU_ACCESS_READ标签
D3D11_MAP_WRITE						映射到内存的资源用于写入。该资源在创建的时候必须绑定了D3D11_CPU_ACCESS_WRITE标签
D3D11_MAP_READ_WRITE				映射到内存的资源用于读写。该资源在创建的时候必须绑定了D3D11_CPU_ACCESS_READ和D3D11_CPU_ACCESS_WRITE标签
D3D11_MAP_WRITE_DISCARD				映射到内存的资源用于写入，之前的资源数据将会被抛弃。该资源在创建的时候必须绑定了D3D11_CPU_ACCESS_WRITE和D3D11_USAGE_DYNAMIC标签
D3D11_MAP_WRITE_NO_OVERWRITE		映射到内存的资源用于写入，但不能复写已经存在的资源。该枚举值只能用于顶点/索引缓冲区。该资源在创建的时候需要有D3D11_CPU_ACCESS_WRITE标签，在Direct3D 11不能用于设置了D3D11_BIND_CONSTANT_BUFFER标签的资源，但在11.1后可以。具体可以查阅MSDN文档
*/