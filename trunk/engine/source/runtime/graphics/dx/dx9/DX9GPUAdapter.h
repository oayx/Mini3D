
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/GPUAdapter.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX9GPUAdapterInfo Final : public GPUAdapterInfo
{
	friend class DX9GPUAdapter;
	BEGIN_DERIVED_REFECTION_TYPE(DX9GPUAdapterInfo, GPUAdapterInfo);
	END_DERIVED_REFECTION_TYPE;

	DEFAULT_CREATE(DX9GPUAdapterInfo);
	FRIEND_CONSTRUCT_DESTRUCT(DX9GPUAdapterInfo);

	DX9GPUAdapterInfo(int index);

public:
	virtual bool IsValid() const override
	{
		return _index != INVALID_INDEX;
	}
	virtual uint GetVendorId() const override
	{
		return (uint)_adapterInfo.VendorId;
	}
	virtual uint GetDeviceId() const override
	{
		return (uint)_adapterInfo.DeviceId;
	}
	virtual String GetDescription() const override
	{
		return (_adapterInfo.Description);
	}

private:
	D3DADAPTER_IDENTIFIER9 _adapterInfo;
};

/********************************************************************/
class DX9GPUAdapter : public GPUAdapter
{
	DEFAULT_CREATE(DX9GPUAdapter);
	FRIEND_CONSTRUCT_DESTRUCT(DX9GPUAdapter);
	BEGIN_DERIVED_REFECTION_TYPE(DX9GPUAdapter, GPUAdapter)
	END_DERIVED_REFECTION_TYPE;

public:
	virtual GPUAdapterInfo* SelectAdapters(void* instance)override;

private:
	void EnumAdapters(void* instance);
};
DC_END_NAMESPACE