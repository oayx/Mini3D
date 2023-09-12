
/*****************************************************************************
* Author： hannibal
* Date：2009/11/20
* Description： 资源相关的定义 
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 资源类型
enum class ResourceType
{
	Undefined  = 0,		//未定义
	Texture,			//纹理
	Material,			//材质
	Mesh,				//模型
	Shader,				//GPU程序
	AudioClip,			//声音
	Video,				//视频
	Txt,				//文本
	Binary,				//二进制
	Font,				//字体
	Scene,				//场景
	SpriteAtlas,		//图集(属于Texture)

	Max
};
DECLARE_ENUM_OPERATORS(ResourceType);

// 资源加载状态
enum class ResLoadState
{
	Undefined = 0,		//未定义 - 不能使用
	Loading,			//加载中 - 不能使用
	Loaded,				//已加载 - 可以使用
};
DECLARE_ENUM_OPERATORS(ResLoadState);

//事件
inline const String RefreshAssetsEvt = "RefreshAssetsEvt";

DC_END_NAMESPACE