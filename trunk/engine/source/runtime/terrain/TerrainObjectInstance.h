 
/*****************************************************************************
* Author： hannibal
* Date：2021/10/6
* Description：地形上Object实例
*****************************************************************************/
#pragma once

#include "runtime/mesh/MeshRender.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL TerrainObjectInstance final : public MeshRender
{
	friend class GameObject;
	FRIEND_CONSTRUCT_DESTRUCT(TerrainObjectInstance);
	DECLARE_OBJECT_SERIALIZE(TerrainObjectInstance);
	BEGIN_DERIVED_REFECTION_TYPE(TerrainObjectInstance, MeshRender)
		CTORS(DEFAULT_CTOR(TerrainObjectInstance))
		INSERT_TYPE_FLAGS(TypeFlag::AllowMultipleComponent)
		ADD_EDITOR_COMPONENT("Terrain/ObjectInstance")
	END_REFECTION_TYPE;

	TerrainObjectInstance() = default;

protected:
	virtual void Start()override;
	virtual void OnDrawEditor()override;

private:
	void GenerateObjects();

protected:
	float	_density = 1.0f;					//数树的密度
	uint	_probability = 100;					//出现概率(百分比)

	Vector3	_positionMin = Vector3::zero;		//位置随机最小值
	Vector3	_positionMax = Vector3::zero;		//位置随机最大值
	Vector3	_scaleMin = Vector3::one;
	Vector3	_scaleMax = Vector3::one;
	Vector3	_rotateMin = Vector3::zero;
	Vector3	_rotateMax = Vector3::zero;
};
DC_END_NAMESPACE