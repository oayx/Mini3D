
/*****************************************************************************
* Author： hannibal
* Date：2009/12/2 
* Description： 场景管理基类
*****************************************************************************/
#pragma once

#include "Scene.h"
#include "runtime/CommonDefine.h"
#include "runtime/graphics/RenderDefine.h"
#include "runtime/component/GameObject.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Light;
class Pass;
class Transform;
class Camera;
class RenderQueue;
class Renderer;
class Frustum;
class Sprite;
class GameObject;
class UICanvas;
class RenderWindow;
class ReflectionProbe;
/********************************************************************/
class ENGINE_DLL SceneManager final : public object
{
	friend class Application;
	friend class Light;
	friend class Camera;
	friend class UICanvas;
	friend class GameObject;
	friend class ReflectionProbe;
	typedef Vector<Scene*> Scenes;
	typedef Vector<Light*> Lights;
	typedef Vector<Camera*> Cameras;
	typedef Vector<UICanvas*> UICanvases;
	typedef List<GameObject*> Objects;
	typedef Map<uint64, GameObject*> MapObjects;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(SceneManager);
	BEGIN_REFECTION_TYPE(SceneManager)
	END_FINAL_REFECTION_TYPE;
	
public:
	static void		Initialize();
	static void		Destroy();

public://更新场景
	static void 	Update();
	static void		UpdateScene();
	static void		UpdateUI();
	static void		Resize(const WindowResizeDesc& desc);

public://渲染
	static void		Render(RenderWindow* window);
	static void 	PreRender(RenderWindow* window);
	static void 	RenderScene(RenderWindow* window);
	static void 	RenderGUI(RenderWindow* window);
	static void 	PostRender(RenderWindow* window);

	static void		AddDrawTriangles(int n){ _nDrawTriangles += n; }
	static void		AddRenderBatch(int n) { _nRenderBatch += n; }
	static void		AddSetPassCall(int n) { _nSetPassCall += n; }
	static void		AddDrawCall(int n) { _nDrawCall += n; }
	static uint		GetDrawTriangles(){ return _oldDrawTriangles; }
	static uint		GetRenderBatch(){ return _oldRenderBatch; }
	static uint		GetSetPassCall(){ return _oldSetPassCall; }
	static uint		GetDrawCall(){ return _oldDrawCall; }

public:
	static void 	SetShadeMode(ShadeMode mode) { _shadeMode = mode; }
	static ShadeMode GetShadeMode(){return  _shadeMode; }

public:
	static GameObject*		GetEngineObject();

	static int				GetCameraCount() { return _cameras.Size(); }
	static Camera*			GetCamera(int index);
	static Camera*			GetCamera(const String& name);
	static void				SetMainCamera(Camera* camera) { _mainCamera = camera; }
	static Camera*			GetMainCamera();
	static ReflectionProbe* GetProbe();

	static Light*			GetLight(const String& name);
	static Light*			GetLight(int index);
	static int				GetLightCount(){return _lights.Size(); }

	static int				GetUICanvasCount() { return _uICanvas.Size(); }
	static UICanvas*		GetUICanvas(int index) { CHECK_RETURN_PTR_NULL(index >= 0 && index < _uICanvas.Size()); return  _uICanvas[index]; }

	static GameObject*		 GetGameObject(uint64 id);
	static List<GameObject*> FindGameObjects(const Ray& ray);

public://场景
	static bool			CreateScene(const String& file);
	static bool			LoadScene(const String& file, LoadSceneMode mode);
	static void			UnloadScene(const String& name);
	static void			UnloadScenes();
	static void			SaveScene();
	static void			SaveAsScene(const String& file);
	static Scene*		GetScene(const String& name);
	static Scene*		GetScene(int index);
	static int			GetSceneCount() { return _scenes.Size(); }
	static Scene*		GetCurrScene() { return _currScene; }
	static bool			IsLoadScening() { return _isLoadScening; }

private://特殊集合管理
	static void			AddCamera(Camera *camera);
	static bool			RemoveCamera(Camera *camera);

	static void			AddLight(Light* light);
	static bool			RemoveLight(Light* light);

	static void			AddUICanvas(UICanvas* canvas);
	static bool			RemoveUICanvas(UICanvas* canvas);

	static void			AddGameObject(GameObject* obj);
	static void			RemoveGameObject(GameObject* obj);

private://场景所有对象
	inline static GameObject*	_engineObject = nullptr;	//特殊对象，不能删除
	inline static MapObjects	_objects;
	inline static Objects		_addedObjects;
	inline static Objects		_removedObjects;

private://渲染
	inline static uint			_nDrawTriangles = 0;		//渲染三角形数量
	inline static uint			_nRenderBatch = 0;			//则是会在第一次调用draw行为的时候加1，如果之后渲染状态没有改变，则batch的数量不再增加，但是一次batch内可能会有多次drawcall调用，只是渲染状态没有改变
	inline static uint			_nSetPassCall = 0;			//指的是切换渲染状态（render state）的次数，比如你的shader中如果有多个pass，或者是场景中有不同的material，都会造成渲染状态切换
	inline static uint			_nDrawCall = 0;				//以gles为例，就是调用draw的实际次数，例如drawarray、drawelement，调用一次都会增加
	inline static uint			_oldDrawTriangles = 0;
	inline static uint			_oldRenderBatch = 0;
	inline static uint			_oldSetPassCall = 0;
	inline static uint			_oldDrawCall = 0;

private:
	inline static Cameras		_cameras;					//场景所有camera
	inline static Camera*		_mainCamera = nullptr;

	inline static Lights		_lights;
	inline static UICanvases	_uICanvas;

private:
	inline static ShadeMode		_shadeMode = ShadeMode::Gouraud;

private:
	inline static Scene*		_currScene = nullptr;
	inline static Scenes		_scenes;
	inline static bool			_isLoadScening = false;
};//SceneManager
DC_END_NAMESPACE
