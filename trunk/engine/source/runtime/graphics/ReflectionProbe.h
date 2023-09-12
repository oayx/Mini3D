
/*****************************************************************************
* Author： hannibal
* Date：2020/12/4 
* Description：实时反射
*****************************************************************************/
#pragma once

#include "runtime/scene/Camera.h"

DC_BEGIN_NAMESPACE
class RenderTexture;
/********************************************************************/
enum class ReflectionMode
{
	OnAwake,	//执行一次
	EveryFrame,	//每帧执行
};
DECLARE_ENUM_OPERATORS(ReflectionMode);

// 实时反射，自带相机
class ENGINE_DLL ReflectionProbe Final : protected Camera
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(ReflectionProbe);
	DISALLOW_COPY_ASSIGN(ReflectionProbe);
	BEGIN_DERIVED_REFECTION_TYPE(ReflectionProbe, Camera)
	END_DERIVED_REFECTION_TYPE;

private:
	ReflectionProbe();
	~ReflectionProbe();

	virtual void Start()override;
	virtual void RenderReflectionProbe()override;
	virtual void Resize(const WindowResizeDesc& desc)override { }

public:
	void SetMode(ReflectionMode mode)	{ _mode = mode; }
	void SetResolution(int size) { _resolution = size; }
	void SetAntiAlias(int antiAlias) { _antiAlias = antiAlias; }
	void SetZNear(float z_near)			{ base::SetZNear(z_near); }
	void SetZFar(float z_far)			{ base::SetZFar(z_far); }
	void SetCullMask(uint mask)			{ base::SetCullMask(mask); }
	void SetClearColor(const Color& color) { base::SetClearColor(color); }

	RenderTexture* GetRenderTexture()const { return base::GetRenderTexture(); }

private:
	void RenderImpl();

private:
	ReflectionMode _mode = ReflectionMode::OnAwake;
	int	_resolution = 128;
	int _antiAlias = 1;
};
DC_END_NAMESPACE
