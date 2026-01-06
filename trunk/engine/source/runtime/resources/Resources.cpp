#include "Resources.h" 
#include "core/image/Image.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(Resource, Object);
Resource::Resource(const ResourceType &type)
	: _type(type)
{
}
Resource::~Resource()
{
}
Object* Resource::Clone(Object* newObj)
{
	base::Clone(newObj);
	Resource* obj = dynamic_cast<Resource*>(newObj);
	AssertEx(obj != nullptr,"null object");
	obj->_type = this->_type;
	obj->_resName = this->_resName;
	obj->_resFile = this->_resFile;
	return obj;
}
String Resource::GetInternalDataPath()
{
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_MAC)
	return Path::Combine(Application::GetPersistentDataPath(), _internalPath);
#elif defined(DC_PLATFORM_WASM)
	return Application::GetPersistentDataPath();
#else
	return Application::GetAssetsPath();
#endif
}
String Resource::GetFullDataPath(const String& file) 
{
	String fullPath = "";
#if defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_MAC)
	if (IsInternal(file))
	{
		fullPath = Path::Combine(Path::Combine(Application::GetPersistentDataPath(), _internalPath), file);
	}
	else
#elif defined(DC_PLATFORM_WASM)
	if (IsInternal(file))
	{
		fullPath = Path::Combine(Application::GetPersistentDataPath(), file);
	}
	else
#endif
	{
		fullPath = Path::Combine(Application::GetAssetsPath(), file);
	}
	Debuger::Log("Resource::GetFullDataPath: %s", fullPath.c_str());
	return fullPath;
}
String Resource::GetFullSavePath(const String& file)
{
	return Path::Combine(Application::GetPersistentDataPath(), file);
}
String Resource::GetRelativePath(const String& file)
{
	String path = file;
	int index = file.IndexOf("assets");
	if (index >= 0)
	{
		if (file.Size() >= index + 7)
			path = file.Substring(index + 7);
		else
			path = "";
	}
	return path;
}
ResourceType Resource::GetResourceType(const String& file)
{
	if (file.IsEmpty())return ResourceType::Undefined;

	String ext = Path::GetExtension(file);
	if (ext.IsEmpty())
	{//可能传进来的已经是扩展名
		ext = Path::GetFileName(file);
	}
	if (ext.IsEmpty())return ResourceType::Undefined;
	ext = ext.ToLower();
	if (Image::GetImageType(ext) != ImageType::Undefined)
	{
		return ResourceType::Texture;
	}
	else if (ext == ".material")
	{
		return ResourceType::Material;
	}
	else if (ext == ".shader")
	{
		return ResourceType::Shader;
	}
	else if (ext == ".fbx" || ext == ".obj")
	{
		return ResourceType::Mesh;
	}
	else if (ext == ".wav")
	{
		return ResourceType::AudioClip;
	}
	else if (ext == ".mp4")
	{
		return ResourceType::Video;
	}
	else if (ext == ".txt" || ext == ".json" || ext == ".lua" || ext == ".cs" || ext == ".asset" || ext == ".hlsl")
	{
		return ResourceType::Txt;
	}
	else if (ext == ".bytes")
	{
		return ResourceType::Binary;
	}
	else if (ext == ".ttf")
	{
		return ResourceType::Font;
	}
	else if (ext == ".scene")
	{
		return ResourceType::Scene;
	}

	return ResourceType::Undefined;
}
bool Resource::Load(const String& file)
{ 
	SetResFile(file);
	return LoadFromFile(file);
}
void Resource::SetResFile(const String& file) 
{
	_resFile = file;
	_resName = Path::GetFileNameWithoutExtension(file);
}
DC_END_NAMESPACE
