
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
	END_DERIVED_REFECTION_TYPE;

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
	static void InitComponent(EComponents& parent_component, VecString& names, const String& class_name);

	static void OnSelectObject(uint64 id);
	static void OnUnselectObject(uint64 id);

private:
	static LayoutType		_layoutType;		//修改模式

	static InspectorType	_inspectorType;
	static uint64			_inspectorId;
	static String			_inspectorName;

	static EComponents		_components;

	static std::atomic<bool> _isWatchFolderChange;	//监视的目录有改变
};
DC_END_NAMESPACE