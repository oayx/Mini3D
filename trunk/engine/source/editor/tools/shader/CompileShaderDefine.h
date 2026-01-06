 
/*****************************************************************************
* Author： hannibal
* Date：2022/12/21
* Description：编译shader定义文件
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/

inline const String HLSLVersion = "5_0";

//编译shader需要的信息
struct sCompileInfo
{
public:
	int pass = 0;
	String hlslVersion = HLSLVersion;
	String path = "";
	ShaderLanguage targetLanguage = ShaderLanguage::Dxil;
	String targetVersion = "";
	String vs_main = Default_VS_Enter;
	String hs_main = Default_HS_Enter;
	String ds_main = Default_DS_Enter;
	String gs_main = Default_GS_Enter;
	String ps_main = Default_PS_Enter;
	String cs_main = Default_CS_Enter;
	bool isVulkanText = false;
	bool srgb = true;
};

//解析的材质信息
struct sShaderInfo
{
public:
	int pass = 0;
	String vsFile = "";
	String vsEnter = "";
	String vsVersion = "";
	String hsFile = "";
	String hsEnter = "";
	String hsVersion = "";
	String dsFile = "";
	String dsEnter = "";
	String dsVersion = "";
	String gsFile = "";
	String gsEnter = "";
	String gsVersion = "";
	String psFile = "";
	String psEnter = "";
	String psVersion = "";
	String csFile = "";
	String csEnter = "";
	String csVersion = "";
	VecString defines;
};
DC_END_NAMESPACE