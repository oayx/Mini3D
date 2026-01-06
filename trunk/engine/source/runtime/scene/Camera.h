/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 
*****************************************************************************/
#pragma once

#include "core/geometry/Frustum.h"
#include "runtime/graphics/null/PostProcess.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/SceneUtils.h"
#include "runtime/scene/LayerMask.h"

DC_BEGIN_NAMESPACE
class Light;
class SkyBox;
class Texture;
class Material;
class RenderTexture;
class RenderQueue;
class RenderDepthMap;
class ReflectionProbe;
/********************************************************************/
// 摄像机
class ENGINE_DLL Camera : public Component
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Camera);
	DISALLOW_COPY_ASSIGN(Camera);
	FRIEND_CONSTRUCT_DESTRUCT(Camera);
	BEGIN_DERIVED_REFECTION_TYPE(Camera, Component)
		CTORS(DEFAULT_CTOR(Camera))
		ADD_EDITOR_COMPONENT("Camera")
	END_REFECTION_TYPE;

protected:
	explicit Camera();
	~Camera();

	virtual void	OnAddComponent(Component* com)override;
	virtual void	OnRemoveComponent(Component* com)override;
	virtual void	OnTransformChanged()override { _dirtyViewMatrix = true; _dirtyFrustum = true; }
	virtual void	OnDrawEditor()override;
	virtual void	OnDrawGizmos(Camera* camera)override;

public:
	const Matrix4&	GetViewMatrix();
	const Matrix4&	GetProjMatrix();
	const Matrix4&	GetViewProjMatrix();

	float			GetFov()const { return _fFov; }
	void  			SetFov(float fov) { _fFov = fov; _dirtyProjMatrix = true; }
	float 			GetAspect()const { return _fAspect; }
	void  			SetAspect(float aspect) { _fAspect = aspect; _dirtyProjMatrix = true; }
	float 			GetZNear()const { return _fZNear; }
	void  			SetZNear(float z_near) { _fZNear = z_near; _dirtyProjMatrix = true; }
	float 			GetZFar()const { return _fZFar; }
	void  			SetZFar(float z_far) { _fZFar = z_far; _dirtyProjMatrix = true; }
	void  			SetFieldOfView(float degree) { AssertEx(degree >= 1 && degree <= 179, "%d", degree); _fFov = degree * Math::Deg2Rad; _dirtyProjMatrix = true; }//1-179度

	void  			SetOrthographic(bool b) { _isOrthographic = b; _dirtyProjMatrix = true; }
	bool  			IsOrthographic()const { return _isOrthographic; }
	void			SetOrthographicSize(float w, float h) { _orthographicSize.Set(w, h); _dirtyProjMatrix = true; }
	const Size&		GetOrthographicSize() const { return _orthographicSize; }

	virtual void	Resize(const WindowResizeDesc& desc);

	void 			SetFillMode(FillMode mode) { _fillMode = mode; }
	FillMode		GetFillMode() { return  _fillMode; }

public://场景
	virtual void	FindVisibleObjects();//获取相机可见对象
	virtual void	AddRenderable(Renderer *obj);
	virtual void	ClearRenderable();//清空渲染队列
	const Aabb&		GetRenderableBoundingBox()const { return _renderableBoundingBox; }

	Frustum*		GetFrustum();//视锥
	FrustumVisible	GetVisibility(const Aabb &bound);//是否在相机视锥
	Vector3v		GetFrustumCornersInWorldSpace();//摄像机视口的八个顶点在世界空间中的位置

	int				GetRenderTargetWidth() const;//渲染目标大小
	int				GetRenderTargetHeight() const;
	//屏幕左下角(0,0)，右上角(1,1)
	Vector3			ScreenToViewportPoint(const Vector3& position);
	Vector3			ViewportToScreenPoint(const Vector3& position);
	Vector3			ScreenToWorldPoint(const Vector3& position);
	Vector3			WorldToScreenPoint(const Vector3& position);
	Vector3			WorldToViewportPoint(const Vector3& position);
	Ray				ScreenPointToRay(const Vector3& position);

public://渲染+视口
	void			SetClearFlag(ClearFlag flag);
	void			SetClearColor(const Color& color) { _clearColor = color; SetClearFlag(ClearFlag::SolidColor); }
	ClearFlag		GetClearFlag()const { return _clearFlag; }
	const Color&	GetClearColor()const { return _clearColor; }

	void			SetSkyBox(const String& file = "");//天空盒
	Texture*		GetSkyBoxTexture()const;

	void			SetViewport(float w, float h);//范围为[0,1]
	void			SetViewport(float x, float y, float w, float h);//范围为[0,1]
	void			SetViewport(float x, float y, float w, float h, float min_z, float max_z);//范围为[0,1]
	ViewPortDesc	GetViewPort()const;

	void			SetDepth(int depth){ _nDepth = depth; }//深度
	int				GetDepth()const{ return _nDepth; }

	void			SetCullMask(uint mask) { _cullMask = mask; }
	uint			GetCullMask()const { return _cullMask; }

	void			SetTargetDisplay(TargetDisplay display) { _targetDisplay = display; }
	TargetDisplay	GetTargetDisplay()const { return _targetDisplay; }

	virtual bool	CanRemove()override;
	virtual void	PreRender();
	virtual void	Render();
	virtual void	PostRender();
	virtual void	RenderNormal();
	virtual void	RenderToTexture(RenderTexture* render_texture);
	virtual void	RenderDepthTexture();
	virtual void	RenderShadowTexture(Light* light);
	virtual void	RenderReflectionProbe() {}

public://渲染到贴图
	bool			CreateRenderTexture(const TextureDesc& desc);
	RenderTexture*	GetRenderTexture()const { return _renderTexture; }

	void			AddDepthTextureMode(DepthTextureMode mode);
	void			RemoveDepthTextureMode(DepthTextureMode mode);
	void			ClearDepthTextureMode();
	DepthTextureMode GetDepthTextureMode()const { return _depthTextureMode; }
	RenderTexture*	GetDepthTexture()const;

	bool			AddPostEffect(const Type* type);
	void			RemovePostEffect(const Type* type);
	void			ClearPostEffect();

	void			EnableHDR(bool enable);
	void			EnableMSAA(bool enable);

public://移动+旋转
	void 			SetPosition(float x, float y, float z);
	void 			SetPosition(const Vector3& pos);

	void 			MoveLeftRight(float d);		//左右移动
	void 			MoveForwardBack(float d);	//前进
	void 			MoveUpDown(float d);		//上下移动

	void 			Pitch(float degree);		// 上下观察
	void 			Yaw(float degree);			// 左右观察
	void 			Rotate(float x_degree, float y_degree);

protected://变换
	Matrix4			_matView = Matrix4::identity;
	Matrix4			_matProj = Matrix4::identity;
	Matrix4			_matViewProj = Matrix4::identity;

	Aabb			_renderableBoundingBox;	//可渲染对象最大包围盒

protected://投影相关
	float			_fFov = Math::PI / 4.0f;
	float			_fAspect = 1;
	float			_fZNear = 0.1f;
	float			_fZFar = 1000.0f;

	bool			_isOrthographic = false;	//是否正交投影
	Size			_orthographicSize;

	bool			_dirtyViewMatrix = true;
	bool			_dirtyProjMatrix = true;
	bool			_dirtyFrustum = true;		//视锥体改变

	FillMode		_fillMode = FillMode::Solid;

protected://清除背景缓存+视口
	TargetDisplay	_targetDisplay = TargetDisplay::Display1;

	ClearFlag		_clearFlag = ClearFlag::SolidColor;
	Color			_clearColor = Color::Clear;//配合_clearFlag用于清除背景缓存

	Frustum*		_frustum = nullptr;
	ViewPortDesc	_viewPort;					//宽度和高度为1表示全部占满
	int				_nDepth = 0;				//控制相机渲染先后
	uint			_cullMask = LayerMask::Everything;

protected:
	RenderQueue*	_renderQueue = nullptr;
	PostProcess*	_postProcess = nullptr;

	RenderTexture*	_renderTexture = nullptr;
	TextureDesc		_renderTextureDesc;

	bool			_enableHDR = false;
	RenderTexture*	_hdrRenderTexture = nullptr;
	Material*		_hdrMaterial = nullptr;

	bool			_enableMSAA = false;

	RenderDepthMap* _renderDepthMap = nullptr;
	DepthTextureMode _depthTextureMode = DepthTextureMode::None;

	SkyBox*			_skyBox = nullptr;
};
DC_END_NAMESPACE