
/*****************************************************************************
* Author： hannibal
* Description：显卡信息
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
// GPU vendors IDs
#define GPU_VENDOR_ID_AMD 0x1002
#define GPU_VENDOR_ID_INTEL 0x8086
#define GPU_VENDOR_ID_NVIDIA 0x10DE
#define GPU_VENDOR_ID_MICROSOFT 0x1414
/********************************************************************/
class GPUAdapterInfo : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(GPUAdapterInfo, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	uint _index = INVALID_INDEX;
	bool _isSelect = false;

public:
	GPUAdapterInfo(int index) { _index = index; }
	virtual bool IsValid()const = 0;
	virtual uint GetDeviceId()const = 0;
	virtual uint GetVendorId()const = 0;
	virtual String GetDescription()const = 0;

public:
	void SetSelect()
	{
		_isSelect = true;
	}
	bool IsSelect()const
	{
		return _isSelect;
	}
	uint GetIndex()const
	{
		return _index;
	}
	bool IsAMD() const//PC
	{
		return GetVendorId() == GPU_VENDOR_ID_AMD;
	}
	bool IsIntel() const//PC
	{
		return GetVendorId() == GPU_VENDOR_ID_INTEL;
	}
	bool IsNVIDIA() const//PC
	{
		return GetVendorId() == GPU_VENDOR_ID_NVIDIA;
	}
	bool IsMicrosoft() const//PC
	{
		return GetVendorId() == GPU_VENDOR_ID_MICROSOFT;
	}
	bool IsPowerVR()const//移动：Apple,iPad,iOS
	{
		return false;
	}
	bool IsMali()const//移动：ARM
	{
		return false;
	}
	bool IsAdreno()const//移动：高通
	{
		return false;
	}
	bool IsTegra()const//移动：nVIDIA
	{
		return false;
	}
};
/********************************************************************/
class GPUAdapter : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(GPUAdapter, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	GPUAdapter() = default;
	~GPUAdapter()
	{
		for (auto adapter : _gPUAdapters)
		{
			SAFE_DELETE(adapter);
		}
		_gPUAdapters.Clear();
	}

public:
	virtual GPUAdapterInfo* SelectAdapters(void* instance) = 0;
	GPUAdapterInfo* GetCurrAdapter()const
	{
		for (const auto& adapter : _gPUAdapters)
		{
			if (adapter->IsSelect())return adapter;
		}
		return nullptr;
	}
	
protected:
	Vector<GPUAdapterInfo*> _gPUAdapters;	//显卡列表
};
DC_END_NAMESPACE