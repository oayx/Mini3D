
/*****************************************************************************
* Author： hannibal
* Date：2020/9/2
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL LayerMask Final : public Object
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
	static const int MAX_NAME_SIZE = 20;
	static String Default;
	static String UI;
	static String IgnoreRaycast;

public:
	static uint Nothing;
	static uint Everything;

private:
	static Layeres _layeres;
};//Light
DC_END_NAMESPACE
