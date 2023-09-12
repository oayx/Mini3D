#include "DX9GPUAdapter.h"
#include "DX9Device.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX9GPUAdapterInfo, GPUAdapterInfo);
IMPL_DERIVED_REFECTION_TYPE(DX9GPUAdapter, GPUAdapter);
DX9GPUAdapterInfo::DX9GPUAdapterInfo(int index)
	: base(index)
{
	DX9HR(GetDX9Device()->GetDirect3D()->GetAdapterIdentifier((UINT)index, 0, &_adapterInfo));
	_adapterInfo.Driver[MAX_DEVICE_IDENTIFIER_STRING - 1] = 0;
	_adapterInfo.Description[MAX_DEVICE_IDENTIFIER_STRING - 1] = 0;
	_adapterInfo.DeviceName[31] = 0;
}
/********************************************************************/
GPUAdapterInfo* DX9GPUAdapter::SelectAdapters(void* instance)
{
	if (_gPUAdapters.IsEmpty())
	{
		this->EnumAdapters(instance);
	}
	if (!_gPUAdapters.IsEmpty())
	{
		return _gPUAdapters[0];
	}

	return nullptr;
}
void DX9GPUAdapter::EnumAdapters(void* instance)
{
	DX9GPUAdapterInfo* adapter = DX9GPUAdapterInfo::Create(0);
	_gPUAdapters.Add(adapter);
}
DC_END_NAMESPACE