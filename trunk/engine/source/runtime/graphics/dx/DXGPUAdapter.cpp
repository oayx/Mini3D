#include "DXGPUAdapter.h"
#include <d3d11.h>

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DXGPUAdapterInfo, GPUAdapterInfo);
IMPL_DERIVED_REFECTION_TYPE(DXGPUAdapter, GPUAdapter);
// 尝试创建硬件设备
static bool TryCreateDevice(IDXGIAdapter* adapter, D3D_FEATURE_LEVEL* max_feature_evel)
{
	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	HRESULT hr = D3D11CreateDevice(
		adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		0,
		feature_levels,
		ARRAY_SIZE(feature_levels),
		D3D11_SDK_VERSION,
		nullptr,
		max_feature_evel,
		nullptr);
	return hr == S_OK;
}
GPUAdapterInfo* DXGPUAdapter::SelectAdapters(void* instance)
{
	IDXGIFactory* factory = (IDXGIFactory*)instance;

	if (_gPUAdapters.IsEmpty())
	{
		this->EnumAdapters(instance);
	}
	if (!_gPUAdapters.IsEmpty())
	{
		IDXGIAdapter* selected_adapter = nullptr;
		int adapter_index = -1;
		for (int i = 0; i < _gPUAdapters.Size(); ++i)
		{
			GPUAdapterInfo* adapter = _gPUAdapters[i];
			if (!adapter->IsValid())
			{
				Debuger::Warning("Failed to choose valid DirectX adapter!");
				continue;
			}
			if (factory->EnumAdapters(adapter->GetIndex(), &selected_adapter) == DXGI_ERROR_NOT_FOUND || selected_adapter == nullptr)
			{
				Debuger::Warning("Cannot get the adapter:%s", adapter->GetDescription().c_str());
				adapter_index = -1;
				continue;
			}
			adapter_index = i;
			break;
		}
		if (adapter_index >= 0)
		{
			return _gPUAdapters[adapter_index];
		}
	}
	return nullptr;
}
void DXGPUAdapter::EnumAdapters(void* instance)
{
	IDXGIFactory* factory = (IDXGIFactory*)instance;

	IDXGIAdapter* tmpAdapter;
	for (uint index = 0; factory->EnumAdapters(index, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; index++)
	{
		D3D_FEATURE_LEVEL max_feature_evel;
		if (tmpAdapter && TryCreateDevice(tmpAdapter, &max_feature_evel))
		{
			DXGPUAdapterInfo* adapter = DXGPUAdapterInfo::Create(index);
			adapter->FeatureLevel = max_feature_evel;
			tmpAdapter->GetDesc(&adapter->Description);

			_gPUAdapters.Add(adapter);
		}
	}
}
DC_END_NAMESPACE