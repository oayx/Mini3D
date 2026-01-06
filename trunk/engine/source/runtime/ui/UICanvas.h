
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "UIUtils.h"
#include "UIImage.h"
#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
class Camera;
class UIView;
class Material;
class Transform;
/********************************************************************/
class ENGINE_DLL UICanvas final : public Renderer
{
	friend class GameObject;
	friend class Application;
	friend class SceneManager;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UICanvas);
	DISALLOW_COPY_ASSIGN(UICanvas);
	FRIEND_CONSTRUCT_DESTRUCT(UICanvas);
	BEGIN_DERIVED_REFECTION_TYPE(UICanvas, Renderer)
		CTORS(DEFAULT_CTOR(UICanvas))
		ADD_EDITOR_COMPONENT("UI/Canvas")
	END_REFECTION_TYPE;

protected:
	UICanvas();
	~UICanvas();

	virtual void	Awake()override;
	virtual void	OnDestroy()override;
	virtual bool	CanRemove()override;
	virtual void	OnDrawEditor()override;
	virtual void	OnDrawGizmos(Camera* camera)override;
	virtual void	Update()override;
	virtual const	Matrix4& GetWorldMatrix()override { return Matrix4::identity; }
	void			Refresh();

public:
	void			SetRenderMode(UIRenderMode mode);
	void			SetCamera(Camera* camera);

	UIRenderMode	GetRenderMode()const { return _renderMode; }
	bool			ScreenToUIPoint(const Vector3& position, Vector2& uiPos);
	void			SetNeedRebuild(bool b) { _isNeedRebuild = b; }

private:
	void			GetViewsInChildren(Transform* node, List<UIView*>& coms) const;
	Material*		GetMaterial(Primitive* primitive, int index);

private:
	bool			_isNeedRebuild = true;

	Camera*			_camera = nullptr;
	UIRenderMode	_renderMode = UIRenderMode::None;
};
DC_END_NAMESPACE