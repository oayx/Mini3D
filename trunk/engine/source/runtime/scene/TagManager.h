
/*****************************************************************************
* Author： hannibal
* Date：2021/8/23
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL TagManager final : public Object
{
	friend class Application;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(TagManager);

public:
	static bool AddTag(const String& name);
	static bool RemoveTag(int index);
	static bool RemoveTag(const String& name);
	static bool ModifyTag(int index, const String& name);
	static int  GetTagCount() { return _tags.Size(); }
	static const String& GetTag(int index) { AssertEx(index >= 0 && index < _tags.Size(),"Arg out of range"); return _tags[index]; }

	static void Load();
	static void Save();

public:
	static constexpr int MAX_NAME_SIZE = 20;

private:
	inline static Vector<String> _tags;
};//Light
DC_END_NAMESPACE
