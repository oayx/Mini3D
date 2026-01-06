
/*****************************************************************************
* Author： hannibal
* Description：编辑器通用配置
*****************************************************************************/
#pragma once

#include "EditorDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class EditorConfig
{
public:
	static void Load();
	static void Save();

public:
	inline static bool	IsLocal = true;			//Local+World
	inline static bool  ShowGrid = true;		//显示网格
	inline static bool	Is3D = true;			//2D+3D
	inline static float OperatorSpeed = 0.6f;	//鼠标操作速度
	inline static int   LayoutIndex = 0.0f;		//布局
	inline static float ProjectIconZoom = 1.0f;	//资源视图
	inline static GameViewResolution GameResolution = GameViewResolution::Free;//Game视图分辨率
};
DC_END_NAMESPACE