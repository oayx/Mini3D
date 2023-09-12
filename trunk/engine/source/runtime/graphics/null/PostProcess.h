 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/16
* Description：后处理
*****************************************************************************/
#pragma once

#include "RenderTexture.h"
#include "runtime/component/Component.h"

DC_BEGIN_NAMESPACE
class Camera;
class Material;
class VariablePrimitive;
/********************************************************************/
// 单个后处理效果
class ENGINE_DLL PostEffect : public Object
{
	friend class PostProcess;
	DISALLOW_COPY_ASSIGN(PostEffect);
	DECLARE_OBJECT_SERIALIZE(PostEffect);
	BEGIN_DERIVED_REFECTION_TYPE(PostEffect, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	PostEffect();
	virtual ~PostEffect();

	virtual void Render(Camera* camera, RenderTexture* source, RenderTexture* dest);
	virtual void OnRenderImage(RenderTexture* source, RenderTexture* dest);
	virtual void OnDrawEditor();

protected:
	void Blit(Texture* source, RenderTexture* dest);
	void Blit(Texture* source, RenderTexture* dest, Material* mat);
	void Blit(Texture* source, RenderTexture* dest, Material* mat, int pass);

	bool IsActive()const { return _active; }
	void SetActive(bool b);

	int  GetPriority()const { return _priority; }

protected:
	bool _active = false;
	int  _priority = 0;		//执行顺序，越大越先执行(unity后处理顺序:Anti-aliasing->Blur->DepthOfField->Bloom->LensDistortion->Vignette->Grain->ColorGrading)
	Camera* _camera = nullptr;
};
/********************************************************************/
// 最后显示到屏幕
class FinalProcess : public Object
{
	friend class PostProcess;
	FRIEND_CONSTRUCT_DESTRUCT(FinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(FinalProcess, Object)
	END_DERIVED_REFECTION_TYPE;

protected:
	FinalProcess() {}
	~FinalProcess() {}
	virtual void Render(Camera* camera, RenderTexture* dest) = 0;
};
/********************************************************************/
// 后处理
class ENGINE_DLL PostProcess Final : public Component
{
	friend class Camera;
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(PostProcess);
	DISALLOW_COPY_ASSIGN(PostProcess);
	FRIEND_CONSTRUCT_DESTRUCT(PostProcess);
	BEGIN_DERIVED_REFECTION_TYPE(PostProcess, Component)
		CTORS(DEFAULT_CTOR(PostProcess))
		ADD_EDITOR_COMPONENT("PostProcess")
	END_DERIVED_REFECTION_TYPE;

	PostProcess() = default;
	virtual ~PostProcess();

protected:
	virtual void Awake()override;
	virtual void Resize(int w, int h);
	virtual void Render(Camera* camera);
	virtual bool CanAdd(GameObject* object)override;
	virtual void OnDrawEditor()override;

public:
	PostEffect* AddEffect(const Type* type);
	PostEffect* GetEffect(const Type* type);
	void RemoveEffect(const Type* type);
	void ClearEffect();
	bool HasEffect()const;
	bool HasEffect(const Type* type)const;
	RenderTexture* GetRenderTexture()const { return _renderTexture; }

private:
	void InitEffectTypes();

protected:
	RenderTexture*		_renderTexture = nullptr;
	FinalProcess*		_finalProcess = nullptr;
	RenderTexture*		_destTexture = nullptr;
	List<PostEffect*>	_effects;
	Vector<const Type*>	_effectTypes;
};
DC_END_NAMESPACE

/*
unity post process 处理顺序
Anti-aliasing
Blur
Builtins:
DepthOfField
Uber Effects:
AutoExposure
LensDistortion
CHromaticAberration
Bloom
Vignette
Grain
ColorGrading(tonemapping)
*/