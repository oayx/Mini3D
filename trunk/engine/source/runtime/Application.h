
/*****************************************************************************
* Author： hannibal
* Date：2009/12/8
* Description： 
*****************************************************************************/
#pragma once

#include "CommonDefine.h"
#include "runtime/component/GameObject.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/graphics/null/RenderContent.h"
#include "runtime/scene/SceneManager.h"
#include "platform/RenderWindow.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Application Final : public Object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(Application);
	BEGIN_DERIVED_REFECTION_TYPE(Application, Object)
	END_DERIVED_REFECTION_TYPE;

public:
	static void Initialize(bool editor = false);
	static void Shutdown();
	static void Start();
	static void Quit();
	static void Pause(bool pause);

	static void MainLoop(int fps);

	static bool IsEditor() { return _isEditor; }
	static bool IsPlay() { return _isPlay; }
	static void SetPlay(bool b) { _isPlay = b; }
	static bool IsPause() { return _isPause; }
	static void SetPause(bool b) { _isPause = b; }

private:
	static void PreUpdate();
	static void Update();
	static void LateUpdate();

public://window
	static bool CreateRenderWindow(WindowDesc& info);
	static void Resize(WindowResizeDesc& desc);

	static GraphicsDevice* GetGraphics() { return _graphicsDevice; }

public:
	static void SetAssetsPath(const String& path);
	static void SetPersistentDataPath(const String& path);
	static const String& GetAssetsPath() { return _readRootPath; }
	static const String& GetPersistentDataPath() { return _saveRootPath; }

	static float GetFrameTime() { return _frameTime / 1000.0f; }

private:
	static GraphicsDevice* _graphicsDevice;

	static String _readRootPath;			//read root path
	static String _saveRootPath;			//write data path

	static bool _isClosing;				//是否需要关闭
	static bool _isEditor;					//是否编辑器运行
	static bool _isPlay;					//是否播放模式
	static bool _isPause;					//是否播放中暂停


	static int64 _frameTime;				//帧间隔时间(微妙)
};
DC_END_NAMESPACE