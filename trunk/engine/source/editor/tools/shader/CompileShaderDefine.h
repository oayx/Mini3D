 
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
	String hlsl_version = HLSLVersion;
	String path = "";
	ShaderLanguage target_language = ShaderLanguage::Dxil;
	String target_version = "";
	String vs_main = Default_VS_Enter;
	String hs_main = Default_HS_Enter;
	String ds_main = Default_DS_Enter;
	String gs_main = Default_GS_Enter;
	String ps_main = Default_PS_Enter;
	String cs_main = Default_CS_Enter;
	bool vulkan_text = false;
	bool srgb = true;
};

//解析的材质信息
struct sShaderInfo
{
public:
	int pass = 0;
	String vs_file = "";
	String vs_enter = "";
	String vs_version = "";
	String hs_file = "";
	String hs_enter = "";
	String hs_version = "";
	String ds_file = "";
	String ds_enter = "";
	String ds_version = "";
	String gs_file = "";
	String gs_enter = "";
	String gs_version = "";
	String ps_file = "";
	String ps_enter = "";
	String ps_version = "";
	String cs_file = "";
	String cs_enter = "";
	String cs_version = "";
	VecString defines;
};
DC_END_NAMESPACE