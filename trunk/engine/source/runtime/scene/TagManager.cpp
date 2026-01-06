#include "TagManager.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
static const String TagFilePath = "settings/TagManager.asset";
/********************************************************************/
bool TagManager::AddTag(const String& name)
{
	if (_tags.Contains(name))
		return false;
	_tags.Add(name);
	return true;
}
bool TagManager::RemoveTag(int index)
{
	if (index < 0 || index >= _tags.Size())return false;
	_tags.RemoveAt(index);
	return true;
}
bool TagManager::RemoveTag(const String& name)
{
	if (!_tags.Contains(name))
		return false;
	_tags.Remove(name);
	return true;
}
bool TagManager::ModifyTag(int index, const String& name)
{
	if (index < 0 || index >= _tags.Size())return false;
	_tags[index] = name;
	return true;
}
void TagManager::Load()
{
	String fullPath = Resource::GetFullDataPath(TagFilePath);
	if (!File::Exist(fullPath))
	{
		return Save();
	}

	_tags.Clear();
	SerializeRead transfer(fullPath);
	{
		if (transfer.Push("Tags"))
		{
			TRANSFER_VECTOR(_tags, "Tag");
			transfer.Pop();
		}
	}
}
void TagManager::Save()
{
	String fullPath = Resource::GetFullDataPath(TagFilePath);
	SerializeWrite transfer(fullPath);
	if (transfer.Push("Tags"))
	{
		TRANSFER_VECTOR(_tags, "Tag");
		transfer.Pop();
	}
}
DC_END_NAMESPACE