
/*****************************************************************************
* Author： hannibal
* Date：2009/12/2
* Description： 
*****************************************************************************/
#pragma once

#include "runtime/component/Component.h"
#include "runtime/scene/SceneUtils.h"
#include "runtime/scene/LayerMask.h"
#include "runtime/graphics/RenderDefine.h"
 
DC_BEGIN_NAMESPACE
class ShadowMap;
/********************************************************************/
class ENGINE_DLL Light final : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Light);
	DISALLOW_COPY_ASSIGN(Light);
	FRIEND_CONSTRUCT_DESTRUCT(Light);
	BEGIN_DERIVED_REFECTION_TYPE(Light, Component)
		CTORS(DEFAULT_CTOR(Light))
		ADD_EDITOR_COMPONENT("Light")
	END_REFECTION_TYPE;

	Light();
	~Light();

protected:
	virtual void	OnDrawEditor()override;

public:
	void			EnableShadowMap(ShadowType type, int w, int h, ColorFormat format);
	void			EnableShadowMap();
	void			DisableShadowMap();
	ShadowMap*		GetShadowMap()const { return _shadowMap; }
	ShadowType		GetShadowType()const { return _shadowType; }
	ShadowType		GetValidShadowType()const;				//目前只能通过.shader文件的Defines宏控制
	void			SetShadowStrength(float strength) { _shadowStrength = strength; }
	float			GetShadowStrength()const { return _shadowStrength; }
	void			SetShadowBias(float bias) { _shadowBias = bias; }
	float			GetShadowBias()const { return _shadowBias; }

	void			SetCullMask(uint mask) { _cullMask = mask; }
	uint			GetCullMask()const { return _cullMask; }

public:
	LightType		mType = LightType::Direction;			//定义灯光类型，我们能够使用下面三种类型之一：D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL
	Color			Diffuse = Color::White;					//此光源发出的颜色。
	float			Range = 10.0f;							//灯光能够传播的最大范围。这个值不能比square root of FLT_MAX大。且不能用于方向光源
	Vector3			Attenuation = Vector3(1, 1.0f,0.1f);	//衰减变量被用来定义灯光强度的传播距离衰减(x定义恒定衰减，y定义线性衰减，z定义二次衰减)，最终公式为：attenuation = 1/(A0 + A1D + A2D*D)
	float			Intensity = 1.0f;						//强度    
	float			SpotAngle = 30;							//聚光灯角度大小[0,180]

private:
	ShadowMap*		_shadowMap = nullptr;
	ShadowType		_shadowType = ShadowType::None;
	float			_shadowStrength = 0.8f;
	float			_shadowBias = 0.0f;

	uint			_cullMask = LayerMask::Everything;
};//Light
DC_END_NAMESPACE
