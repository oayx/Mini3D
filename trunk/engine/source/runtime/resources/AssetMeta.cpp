﻿#include "AssetMeta.h"
#include "AssetsManager.h"
#include "Resources.h"
#include "runtime/font/FontManager.h"
#include "runtime/ui/UIAtlas.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AssetMeta, Object);
AssetMeta* AssetMeta::Create(ResourceType type)
{
	AssetMeta* meta = nullptr;
	switch (type)
	{
	case ResourceType::Texture:		meta = DBG_NEW TextureMeta(); break;
	case ResourceType::Material:	meta = DBG_NEW MaterialMeta(); break;
	case ResourceType::Mesh:		meta = DBG_NEW MeshMeta(); break;
	case ResourceType::Shader:		meta = DBG_NEW ShaderMeta(); break;
	case ResourceType::Font:		meta = DBG_NEW FontMeta(); break;
	case ResourceType::AudioClip:	meta = DBG_NEW AudioMeta(); break;
	case ResourceType::Video:		meta = DBG_NEW VideoMeta(); break;
	default: 						meta = DBG_NEW AssetMeta(); meta->_resType = type; break;
	}
	return meta;
}
AssetMeta::~AssetMeta()
{ 
	for (auto obj : _childrens)
	{
		SAFE_DELETE(obj);
	}
	_childrens.Clear(); 
}
void AssetMeta::SetFileInfo(const FileInfo& file_info, const String& assets_root_path)
{
	_fileName = file_info.FileName;
	_extension = file_info.Extension;
	_fullName = file_info.FullName.Substring(assets_root_path.Size() + 1);//转换成assets相对目录
	_fileType = file_info.FileType;

	if (_fileType == FileInfoType::Dir)
	{
		//子
		_childrens.Clear();
		_childrens.Reserve(file_info.GetChildrenCount());
		for (int i = 0; i < file_info.GetChildrenCount(); ++i)
		{
			const FileInfo& child_file_info = file_info.GetChildren(i);
			ResourceType resource_type = Resource::GetResourceType(child_file_info.FileName);
			AssetMeta* meta = AssetMeta::Create(resource_type);
			_childrens.Add(meta);
		}

		//是否有子文件夹
		for (int i = 0; i < file_info.GetChildrenCount(); ++i)
		{
			if (file_info.GetChildren(i).FileType == FileInfoType::Dir)
			{
				_hasChildrenFolder = true;
				break;
			}
		}
	}
}
bool AssetMeta::HasChildren(const String& name)const
{
	for (int i = 0; i < _childrens.Size(); ++i)
	{
		const AssetMeta* file_info = _childrens[i];
		if (file_info->_fileName.Equals(name, true))
			return true;
	}
	return false;
}
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(AssetMeta);
template<class TransferFunction> inline
void AssetMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	TRANSFER_SIMPLE(_gUID);
	TRANSFER_SIMPLE(_hideInspector);
}
void AssetMeta::Serialize()
{
	String full_path = Resource::GetFullDataPath(_fullName + ".meta");
	SerializeRead transfer(full_path);
	switch (_resType)
	{
	case ResourceType::Texture:		TRANSFER_CLASS(*(this->As<TextureMeta>())); break;
	case ResourceType::Material:	TRANSFER_CLASS(*(this->As<MaterialMeta>())); break;
	case ResourceType::Mesh:		TRANSFER_CLASS(*(this->As<MeshMeta>())); break;
	case ResourceType::Shader:		TRANSFER_CLASS(*(this->As<ShaderMeta>())); break;
	case ResourceType::Font:		TRANSFER_CLASS(*(this->As<FontMeta>())); break;
	case ResourceType::AudioClip:	TRANSFER_CLASS(*(this->As<AudioMeta>())); break;
	case ResourceType::Video:		TRANSFER_CLASS(*(this->As<VideoMeta>())); break;
	default:						TRANSFER_CLASS(*(this)); break;
	}
}
void AssetMeta::Deserialize()
{
	String full_path = Resource::GetFullDataPath(_fullName + ".meta");
	SerializeWrite transfer(full_path);
	switch (_resType)
	{
	case ResourceType::Texture:		TRANSFER_CLASS(*(this->As<TextureMeta>())); break;
	case ResourceType::Material:	TRANSFER_CLASS(*(this->As<MaterialMeta>())); break;
	case ResourceType::Mesh:		TRANSFER_CLASS(*(this->As<MeshMeta>())); break;
	case ResourceType::Shader:		TRANSFER_CLASS(*(this->As<ShaderMeta>())); break;
	case ResourceType::Font:		TRANSFER_CLASS(*(this->As<FontMeta>())); break;
	case ResourceType::AudioClip:	TRANSFER_CLASS(*(this->As<AudioMeta>())); break;
	case ResourceType::Video:		TRANSFER_CLASS(*(this->As<VideoMeta>())); break;
	default:						TRANSFER_CLASS(*(this)); break;
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(TextureMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(TextureMeta);
template<class TransferFunction> inline
void TextureMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_ENUM(_textureGroup);
	TRANSFER_ENUM(_textureType);
	TRANSFER_SIMPLE(m_sRGB);
	TRANSFER_SIMPLE(_enableReadWrite);
	TRANSFER_SIMPLE(_generateMipMaps);
	TRANSFER_ENUM(_addressMode);
	TRANSFER_ENUM(_borderColor);
	TRANSFER_ENUM(_filterType);
	TRANSFER_SIMPLE(_anisotropy);
}
void TextureMeta::SetTextureGroup(TextureGroup group)
{
	if (group == _textureGroup)return;
	if (_textureGroup == TextureGroup::SpriteAtlas)
	{
		AssetsManager::RemoveSpriteAtlas(_fullName);
	}
	if (group == TextureGroup::SpriteAtlas)
	{
		AssetsManager::AddSpriteAtlas(_fullName);
	}
	_textureGroup = group;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MeshMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(MeshMeta);
template<class TransferFunction> inline
void MeshMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	TRANSFER_SIMPLE(_genNormal);
	TRANSFER_SIMPLE(_genTangent);
	TRANSFER_SIMPLE(_optimizeMesh);
	TRANSFER_SIMPLE(_importAnimation);

	if (transfer.Push("Clips"))
	{
		TRANSFER_VECTOR(_clips, "Clip");
		transfer.Pop();
	}
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(MaterialMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(MaterialMeta);
template<class TransferFunction> inline
void MaterialMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(ShaderMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(ShaderMeta);
template<class TransferFunction> inline
void ShaderMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FontMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(FontMeta);
template<class TransferFunction> inline
void FontMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);

	if (!transfer.IsRead())
	{
		if (_fontName.IsEmpty())
		{
			_fontName = Path::GetFileNameWithoutExtension(_fileName);
		}
	}

	TRANSFER_SIMPLE(_fontName);
	TRANSFER_SIMPLE(_fontSize);

	if (transfer.IsRead())
	{
		if (_fontName.IsEmpty())
		{
			_fontName = Path::GetFileNameWithoutExtension(_fileName);
		}
	}
}
void FontMeta::SetFontName(const String& name)
{
	if (!name.IsEmpty())
	{
		_fontName = name;
	}
}
void FontMeta::SetFontSize(int size)
{
	if (size < 0 || size > 512)return;
	_fontSize = size;
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(AudioMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(AudioMeta);
template<class TransferFunction> inline
void AudioMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}

/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VideoMeta, AssetMeta);
INSTANTIATE_TEMPLATE_TRANSFER_WITH_DECL(VideoMeta);
template<class TransferFunction> inline
void VideoMeta::Transfer(TransferFunction& transfer, void* ptr)
{
	base::Transfer(transfer, ptr);
}
DC_END_NAMESPACE