 
/*****************************************************************************
* Author： hannibal
* Date：2021/1/5
* Description：粒子渲染
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
class Camera;
class Particle;
class VariablePrimitive;
class ParticleSystem;
/********************************************************************/
class ENGINE_DLL ParticleRender final : public Renderer
{
	friend class GameObject;
	friend class ParticleSystem;
	FRIEND_CONSTRUCT_DESTRUCT(ParticleRender);
	DISALLOW_COPY_ASSIGN(ParticleRender);
	BEGIN_DERIVED_REFECTION_TYPE(ParticleRender, Renderer)
		CTORS(DEFAULT_CTOR(ParticleRender))
	END_REFECTION_TYPE;

	ParticleRender() = default;

protected:
	virtual void Awake()override;
	virtual void Start()override;
	virtual void Update()override;
	virtual void OnPreRender(Camera* camera)override;
	virtual void OnAddComponent(Component* com)override;
	virtual void OnRemoveComponent(Component* com)override;
	virtual bool CanSerialize()override { return false; }
	virtual const Matrix4& GetWorldMatrix()override { return Matrix4::identity; }//特效渲染不受变换影响
	virtual uint64 GetEditorAction()const override { return DEFAULT_COMPONENT_ACTION | uint64(ComponentAction::Update); }
	virtual void OnDrawEditor()override;

private:
	void CalFrameTime();
	void Refresh(Camera* camera);
	bool FillMesh(Particle* particle, Camera* camera, VariablePrimitive* primitive, int& vxtOffset, int& idxOffset);

private:
	ParticleSystem* _particleSystem = nullptr;
	int		_currFrame = 0;
	float	_frameTime = 0.0f;	//切换序列帧时间间隔
	float	_lastFrameTime = 0.0f;
};
DC_END_NAMESPACE
