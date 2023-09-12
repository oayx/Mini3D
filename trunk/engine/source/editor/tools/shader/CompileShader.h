 
/*****************************************************************************
* Author： hannibal
* Date：2022/12/21
* Description：编译shader
*****************************************************************************/
#pragma once

#include "editor/tools/shader/CompileShaderDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class CompileShader
{
public:
	void StartCompile();

private://编译shader
	void CompilePlatform(const List<ShaderLanguage>& targetLanguages);
	void CompileLanguage(ShaderLanguage target_language);

	void CompileFolder(sCompileInfo info);
	bool CompileFile(sCompileInfo info);

	bool CompileDefine(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const VecString& defines);
	bool CompileSRGB(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& language_name, const String& define);
	bool CompileImpl(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& language_name, const String& define, bool srgb);

private:
	//获得材质包含的shader
	bool GetMaterialShader(const String& material_file, List<sShaderInfo>& list);
	String GetWriteShaderToFilePath(const String& dir_path, const String& file_name, const String& file_ext, const String& target_version, const String& define, const String& language_name, int pass, bool srgb);
	String GetExtensionByShaderStage(ShaderType shader_stage);
	String ShadingLanguageFileName(ShaderLanguage target_language);

private://替换材质
	void ReplaceShaders();
	void ReplaceShader(const String& shader_path);
	int ReplaceShaderImpl(const String& shader_path, const String& hlsl_version);
};
DC_END_NAMESPACE