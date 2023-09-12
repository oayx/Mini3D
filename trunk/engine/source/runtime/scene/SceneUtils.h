
/*****************************************************************************
* Author： hannibal
* Date：2009/12/2 
* Description： 
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
#define MAX_DIRECTION_LIGHT	4	// 单个可渲染对象最多受方向光数量
#define MAX_POINT_LIGHT		2	// 单个可渲染对象最多受点光源数量
#define MAX_SPOT_LIGHT		2	// 单个可渲染对象最多受聚光灯数量
/********************************************************************/
//场景树
enum class SceneTreeType
{
	None = 0,
	Octree,			//八叉树
	Quadtree,		//四叉树
};
DECLARE_ENUM_OPERATORS(SceneTreeType);

// 灯光类型
enum class LightType
{
	Direction = 0,	//方向光
	Point,			//点光源
	Spot,			//聚光灯
};
DECLARE_ENUM_OPERATORS(LightType);

// 渲染顺序
enum class eRenderQueue
{
	Background	= 1000,	//背景
	Geometry	= 2000,	//默认
	AlphaTest	= 2500,	//进行AlphaTest的物体在这个队列
	BTransparent= 2500,	//开始半透明
	Transparent = 3000,	//半透明
	ETransparent= 3500, //结束半透明
	Overlay		= 4000,	//UI
};
DECLARE_ENUM_OPERATORS(eRenderQueue);

//判断是否透明通道
inline bool IsTransparentQueue(int queue)
{
	return queue >= (int)eRenderQueue::BTransparent && queue <= (int)eRenderQueue::ETransparent;
}

// 相机清除标记
enum class ClearFlag : byte
{
	None		= 0x00,
	Color		= 0x01,
	Depth		= 0x02,
	Stencil		= 0x04,
	SBox		= 0x08,

	Skybox		= Color | Depth | Stencil | SBox,	//天空盒
	SolidColor	= Color | Depth | Stencil,			//颜色
	DepthOnly	= Depth,							//深度
	DonotClear	= None,								//不清除
};
DECLARE_ENUM_OPERATORS(ClearFlag);

//相机类型
enum class CameraType
{
	Perspective = 1,
	Orthographic,
};
DECLARE_ENUM_OPERATORS(CameraType);

enum CameraFlags
{
	None	= 0x01,
};
DECLARE_ENUM_OPERATORS(CameraFlags);

//阴影类型
enum class ShadowType
{
	None = 0,
	Hard,
	Soft,
};
DECLARE_ENUM_OPERATORS(ShadowType);
//阴影分辨率
enum class ShadowResolution
{
	Low = 0,
	Medium,
	High,
	VeryHigh,
};
DECLARE_ENUM_OPERATORS(ShadowResolution);

//深度图
enum class DepthTextureMode
{
	None = 0,
	Depth = 0x1,
	Normals = 0x2,
	MotionVectors = 0x4,
};
DECLARE_ENUM_OPERATORS(DepthTextureMode);

//显示器
enum class TargetDisplay
{
	Display1 = 0,
	Display2,
	Display3,
	Display4,
	Display5,
	Display6,
	Display7,
	Display8,
	Max,
};
DECLARE_ENUM_OPERATORS(TargetDisplay);

//场景加载方式
enum class LoadSceneMode
{
	Single = 0,
	Additive = 1,
};
DECLARE_ENUM_OPERATORS(LoadSceneMode);

DC_END_NAMESPACE
