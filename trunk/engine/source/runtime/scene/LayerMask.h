
/*****************************************************************************
* Author： hannibal
* Date：2020/9/2
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL LayerMask final : public Object
{
	typedef Vector<String> Layeres;
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(LayerMask);

public:
	static bool SetLayerName(int layer, const String& name);
	static int NameToLayer(const String& name);
	static const String& LayerToName(int layer);

	static uint GetMask(int layer);
	static uint GetMask(const String& name);
	static uint GetMask(const VecString& names);

	static uint GetAllMask();

	static void Load();
	static void Save();

public:
	inline static constexpr int MAX_NAME_SIZE = 20;
	inline static String Default = "Default";
	inline static String UI = "UI";
	inline static String IgnoreRaycast = "IgnoreRaycast";

public:
	inline static uint Nothing = 0;
	inline static uint Everything = 4294967295;

private:
	inline static Layeres _layeres;
};//Light
DC_END_NAMESPACE
