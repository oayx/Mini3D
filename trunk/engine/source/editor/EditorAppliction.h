
/*****************************************************************************
* Author： hannibal
* Description：编辑器
*****************************************************************************/
#pragma once

#include "EditorConfig.h"
#include "EditorDefine.h"
#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL EditorAppliction : public Object
{
	friend class Application;
	friend class SceneManager;
	DEFAULT_CREATE(EditorAppliction);
	FRIEND_CONSTRUCT_DESTRUCT(EditorAppliction);
	DISALLOW_COPY_ASSIGN(EditorAppliction);
	BEGIN_DERIVED_REFECTION_TYPE(EditorAppliction, Object)
	END_REFECTION_TYPE;

private:
	static void Initialize();
	static void Destroy();
	static void Update();
	static void Render();
	static void Resize(const WindowResizeDesc& desc);

public:
	static bool CreateProject(const String& path);
	static bool OpenProject(const String& path);

public:
	static InspectorType GetInspectorType() { return _inspectorType; }
	static uint64 GetInspectorId() { return _inspectorId; }
	static const String& GetInspectorName() { return _inspectorName; }
	static void SetInspectorType(InspectorType type);
	static void SetInspectorType(InspectorType type, uint64 id);
	static void SetInspectorType(InspectorType type, const String& file);

	static const EComponents& GetComponents() { return _components; }

private:
	static void OnRecvLogMessage(LogMsgType type, const char* msg);
	static void OnWatchFolderChange();

	static void InitComponent();
	static void InitComponent(EComponents& parentComponent, VecString& names, const String& className);

	static void OnSelectObject(uint64 id);
	static void OnUnselectObject(uint64 id);

private:
	inline static LayoutType		_layoutType = LayoutType::Default;		//修改模式

	inline static InspectorType		_inspectorType = InspectorType::None;
	inline static uint64			_inspectorId = 0;
	inline static String			_inspectorName = "";

	inline static EComponents		_components;

	inline static std::atomic<bool> _isWatchFolderChange = false;	//监视的目录有改变
};
DC_END_NAMESPACE