
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
	static bool	 IsLocal;			//Local+World
	static bool  ShowGrid;			//显示网格
	static bool	 Is3D;				//2D+3D
	static float OperatorSpeed;		//鼠标操作速度
	static int   LayoutIndex;		//布局
	static float ProjectIconZoom;	//资源视图
	static GameViewResolution GameResolution;//Game视图分辨率
};
DC_END_NAMESPACE