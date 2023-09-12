
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GPUAdapter.h"
#include "dx11/DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DXGPUAdapterInfo Final : public GPUAdapterInfo
{
	BEGIN_DERIVED_REFECTION_TYPE(DXGPUAdapterInfo, GPUAdapterInfo);
	END_DERIVED_REFECTION_TYPE;

	using base::base;
	DEFAULT_CREATE(DXGPUAdapterInfo);
	FRIEND_CONSTRUCT_DESTRUCT(DXGPUAdapterInfo);

public:
	D3D_FEATURE_LEVEL FeatureLevel;
	DXGI_ADAPTER_DESC Description;

public:
	virtual bool IsValid() const override
	{
		return _index != INVALID_INDEX && FeatureLevel != static_cast<D3D_FEATURE_LEVEL>(0);
	}
	virtual uint GetVendorId() const override
	{
		return (uint)Description.VendorId;
	}
	virtual uint GetDeviceId() const override
	{
		return (uint)Description.DeviceId;
	}
	virtual String GetDescription() const override
	{
		return String::FromWString(Description.Description);
	}
};

/********************************************************************/
class DXGPUAdapter : public GPUAdapter
{
	DEFAULT_CREATE(DXGPUAdapter);
	FRIEND_CONSTRUCT_DESTRUCT(DXGPUAdapter);
	BEGIN_DERIVED_REFECTION_TYPE(DXGPUAdapter, GPUAdapter)
	END_DERIVED_REFECTION_TYPE;

public:
	virtual GPUAdapterInfo* SelectAdapters(void* instance)override;

private:
	void EnumAdapters(void* instance);
};
DC_END_NAMESPACE