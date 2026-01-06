#include "LayerMask.h"
#include "runtime/resources/Resources.h"

DC_BEGIN_NAMESPACE
static const String LayerFilePath = "settings/LayerManager.asset";
/********************************************************************/
bool LayerMask::SetLayerName(int layer, const String& name)
{
	if (layer < 5 || layer >= 32)return false;
	_layeres[layer] = name;
	return true;
}
int LayerMask::NameToLayer(const String& name)
{
	if (_layeres.IsEmpty())return 0;
	if (name.IsEmpty())return 0;
	for (int i = 0; i < _layeres.Size(); ++i)
	{
		if (_layeres[i] == name)
		{
			return i;
		}
	}
	return -1;
}
const String& LayerMask::LayerToName(int layer)
{
	if (_layeres.IsEmpty())return Default;
	AssertEx (layer >= 0 && layer < 32, "Arg out of range");
	return _layeres[layer];
}
uint LayerMask::GetMask(int layer)
{
	if (_layeres.IsEmpty())return 0;
	if (layer < 0 || layer >= 32)return 0;
	return 1 << layer;
}
uint LayerMask::GetMask(const String& name)
{
	if (_layeres.IsEmpty())return 0;
	if (name.IsEmpty())return 0;
	for (uint i = 0; i < _layeres.Size(); ++i)
	{
		if (_layeres[i] == name)
		{
			return 1 << i;
		}
	}
	return 0;
}
uint LayerMask::GetMask(const VecString& names)
{
	if (_layeres.IsEmpty())return 0;
	uint mask = 0;
	for (const auto& name : names)
	{
		for (uint i = 0; i < _layeres.Size(); ++i)
		{
			if (_layeres[i] == name)
			{
				mask |= (1 << i);
			}
		}
	}
	return mask;
}
uint LayerMask::GetAllMask()
{
	if (_layeres.IsEmpty())return 0;
	uint mask = 0;
	for (uint i = 0; i < _layeres.Size(); ++i)
	{
		if (!_layeres[i].IsEmpty())
		{
			mask |= (1 << i);
		}
	}
	return mask;
}
void LayerMask::Load()
{
	_layeres.Clear();
	String fullPath = Resource::GetFullDataPath(LayerFilePath);
	SerializeRead transfer(fullPath);
	{
		if (transfer.Push("Layeres"))
		{
			TRANSFER_VECTOR(_layeres, "Layer");
			transfer.Pop();
		}
	}

	if (_layeres.IsEmpty())
	{//没有文件:初始化
		_layeres.Resize(32);
		_layeres[0] = LayerMask::Default;
		_layeres[1] = LayerMask::UI;
		_layeres[2] = LayerMask::IgnoreRaycast;
		Save();
	}
}
void LayerMask::Save()
{
	String fullPath = Resource::GetFullDataPath(LayerFilePath);
	SerializeWrite transfer(fullPath);
	if (transfer.Push("Layeres"))
	{
		TRANSFER_VECTOR(_layeres, "Layer");
		transfer.Pop();
	}
}
DC_END_NAMESPACE