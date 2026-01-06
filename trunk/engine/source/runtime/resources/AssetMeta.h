
/*****************************************************************************
* Author： hannibal
* Description：meta文件
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/mesh/MeshUtils.h"
#include "runtime/resources/ResourcesDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class AssetMeta : public Object
{
	friend class AssetsManager; 
	FRIEND_CONSTRUCT_DESTRUCT(AssetMeta);
	DECLARE_SERIALIZE(AssetMeta);
	BEGIN_DERIVED_REFECTION_TYPE(AssetMeta, Object)
	END_REFECTION_TYPE;

	static AssetMeta* Create(ResourceType type);

protected:
	AssetMeta() = default;
	~AssetMeta();
	bool operator ==(const AssetMeta& meta)
	{
		return _gUID == meta._gUID && _resType == meta._resType && _fileName == meta._fileName && _fullName == meta._fullName;
	}
	bool operator !=(const AssetMeta& meta)
	{
		return !(*this == meta);
	}

	void SetGUID(const String& guid) { _gUID = guid; }
	void SetFileInfo(const FileInfo& fileInfo, const String& assetsRootPath);

	virtual void Serialize()override;
	virtual void Deserialize()override;

public:
	const String& GetGUID()const { return _gUID; }
	const String& GetFileName()const { return _fileName; }
	const String& GetExtension()const { return _extension; }
	const String& GetFullPath()const { return _fullName; }

	ResourceType  GetResType()const { return _resType; }
	FileInfoType  GetFileType()const { return _fileType; }

	int		GetChildrenCount()const { return _childrens.Size(); }
	const	AssetMeta* GetChildren(int i)const { AssertEx(i < _childrens.Size(), ""); return _childrens[i]; }
			AssetMeta* GetChildren(int i){ AssertEx(i < _childrens.Size(), ""); return _childrens[i]; }
	const	Vector<AssetMeta*>& Children()const { return _childrens; }
	bool	HasChildren(const String& name)const;
	void	DeleteChild(int i) { _childrens.RemoveAt(i); }

	bool	IsHideInspector()const { return _hideInspector; }
	bool	IsFolder()const { return _fileType == FileInfoType::Dir; }
	bool	HasChildrenFolder()const { return _hasChildrenFolder; }//当前文件夹有子文件夹

protected:
	String	_gUID = "";
	String	_fileName = "";			//文件名(带扩展名)
	String	_extension = "";			// 获取表示文件扩展名部分的字符串
	String	_fullName = "";			//相对asset目录
	bool	_hideInspector = false;	//不显示在面板
	bool	_hasChildrenFolder = false;//有子文件夹

	FileInfoType _fileType = FileInfoType::File;
	ResourceType _resType = ResourceType::Undefined;

	Vector<AssetMeta*> _childrens;
};

/********************************************************************/
//图片
class TextureMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	friend class EInspector_Project_Texture;
	DECLARE_SERIALIZE(TextureMeta);
	FRIEND_CONSTRUCT_DESTRUCT(TextureMeta);
	BEGIN_DERIVED_REFECTION_TYPE(TextureMeta, AssetMeta)
	END_REFECTION_TYPE;

	TextureMeta() { _resType = ResourceType::Texture; }
	bool operator ==(const TextureMeta& meta)
	{
		return _textureGroup == meta._textureGroup
			&& _textureType == meta._textureType
			&& _sRGB == meta._sRGB 
			&& _enableReadWrite == meta._enableReadWrite
			&& _generateMipMaps == meta._generateMipMaps
			&& _addressMode == meta._addressMode
			&& _filterType == meta._filterType
			&& _anisotropy == meta._anisotropy;
	}
	bool operator !=(const TextureMeta& meta)
	{
		return !(*this == meta);
	}
	//TextureMeta& operator =(const TextureMeta& meta)
	//{
	//	
	//	return *this;
	//}

public:
	void			SetTextureGroup(TextureGroup group);
	TextureGroup	GetTextureGroup()const { return _textureGroup; }
	TextureType		GetTextureType()const{ return _textureType; }
	bool			IssRGB()const{ return _sRGB; }
	bool			IsEnableReadWrite()const{ return _enableReadWrite; }
	bool			IsGenerateMipMaps()const{ return _generateMipMaps; }
	TextureAddress	GetAddressMode()const{ return _addressMode; }
	const Color&	GetBorderColor()const { return _borderColor; }
	TextureFilter	GetFilterType()const{ return _filterType; }
	uint			GetAnisotropy()const{ return _anisotropy; }

protected:
	TextureGroup	_textureGroup = TextureGroup::Default;
	TextureType		_textureType = TextureType::D2;
	bool			_sRGB = true;
	bool			_enableReadWrite = false;
	bool			_generateMipMaps = false;

	TextureAddress	_addressMode = TextureAddress::Clamp;
	Color			_borderColor = Color::Black;
	TextureFilter	_filterType = TextureFilter::Bilinear;
	uint			_anisotropy = 1;
};

/********************************************************************/
//模型
class MeshMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	friend class EInspector_Project_Mesh;
	DECLARE_SERIALIZE(MeshMeta);
	FRIEND_CONSTRUCT_DESTRUCT(MeshMeta);
	BEGIN_DERIVED_REFECTION_TYPE(MeshMeta, AssetMeta)
	END_REFECTION_TYPE;

	MeshMeta() { _resType = ResourceType::Mesh; }
	bool operator ==(const MeshMeta& meta)
	{
		return _genNormal == meta._genNormal
			&& _genTangent == meta._genTangent
			&& _optimizeMesh == meta._optimizeMesh;
	}
	bool operator !=(const MeshMeta& meta)
	{
		return !(*this == meta);
	}

public:
	bool IsGenNormal()const { return _genNormal; }
	bool IsGenTangent()const { return _genTangent; }
	bool IsOptimizeMesh()const { return _optimizeMesh; }
	bool IsImportAnimation()const { return _importAnimation; }
	const ClipAnimations& GetClips()const { return _clips; }

protected:
	bool _genNormal = true;
	bool _genTangent = true;
	bool _optimizeMesh = true;

	bool _importAnimation = false;
	ClipAnimations _clips;
};

/********************************************************************/
//材质
class MaterialMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	DECLARE_SERIALIZE(MaterialMeta);
	FRIEND_CONSTRUCT_DESTRUCT(MaterialMeta);
	BEGIN_DERIVED_REFECTION_TYPE(MaterialMeta, AssetMeta)
	END_REFECTION_TYPE;

	MaterialMeta() { _resType = ResourceType::Material; }
	bool operator ==(const MaterialMeta& meta)
	{
		return true;
	}
	bool operator !=(const MaterialMeta& meta)
	{
		return !(*this == meta);
	}
};

/********************************************************************/
//shader
class ShaderMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	DECLARE_SERIALIZE(ShaderMeta);
	FRIEND_CONSTRUCT_DESTRUCT(ShaderMeta);
	BEGIN_DERIVED_REFECTION_TYPE(ShaderMeta, AssetMeta)
	END_REFECTION_TYPE;

	ShaderMeta() { _resType = ResourceType::Shader; }
	bool operator ==(const ShaderMeta& meta)
	{
		return true;
	}
	bool operator !=(const ShaderMeta& meta)
	{
		return !(*this == meta);
	}
};

/********************************************************************/
//字体
class FontMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	friend class EInspector_Project_Font;
	DECLARE_SERIALIZE(FontMeta);
	FRIEND_CONSTRUCT_DESTRUCT(FontMeta);
	BEGIN_DERIVED_REFECTION_TYPE(FontMeta, AssetMeta)
	END_REFECTION_TYPE;

	FontMeta() { _resType = ResourceType::Font; }
	bool operator ==(const FontMeta& meta)
	{
		return _fontName == meta._fontName
			&& _fontSize == meta._fontSize;
	}
	bool operator !=(const FontMeta& meta)
	{
		return !(*this == meta);
	}

public:
	void SetFontName(const String& name);
	const String& GetFontName()const { return _fontName; }
	void SetFontSize(int size);
	int GetFontSize()const { return _fontSize; }

protected:
	String  _fontName = "";
	int		_fontSize = 24;
};
/********************************************************************/
//音频
class AudioMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	DECLARE_SERIALIZE(AudioMeta);
	FRIEND_CONSTRUCT_DESTRUCT(AudioMeta);
	BEGIN_DERIVED_REFECTION_TYPE(AudioMeta, AssetMeta)
	END_REFECTION_TYPE;

	AudioMeta() { _resType = ResourceType::AudioClip; }
	bool operator ==(const AudioMeta& meta)
	{
		return true;
	}
	bool operator !=(const AudioMeta& meta)
	{
		return !(*this == meta);
	}

public:

};

/********************************************************************/
//视频
class VideoMeta : public AssetMeta
{
	friend class AssetMeta;
	friend class AssetsManager;
	DECLARE_SERIALIZE(VideoMeta);
	FRIEND_CONSTRUCT_DESTRUCT(VideoMeta);
	BEGIN_DERIVED_REFECTION_TYPE(VideoMeta, AssetMeta)
	END_REFECTION_TYPE;

	VideoMeta() { _resType = ResourceType::Video; }
	bool operator ==(const VideoMeta& meta)
	{
		return true;
	}
	bool operator !=(const VideoMeta& meta)
	{
		return !(*this == meta);
	}
};
DC_END_NAMESPACE