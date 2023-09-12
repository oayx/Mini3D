
/*****************************************************************************
* Author： hannibal
* Description：命令行参数
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL CommandLine Final : public object
{
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(CommandLine);
	BEGIN_REFECTION_TYPE(CommandLine)
	END_FINAL_REFECTION_TYPE;
	
public:
	static void Pause(int argc, char** argv);

private:
	static VecString _commands;
};
DC_END_NAMESPACE