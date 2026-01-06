 
/*****************************************************************************
* Author： hannibal
* Date：2022/12/21
* Description：编译shader
*****************************************************************************/
#pragma once
#if defined(DC_PLATFORM_WIN32) && defined(_WIN64)
#include "editor/tools/shader/CompileShaderDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class CompileShader
{
public:
	void StartCompile();

private://编译shader
	void CompilePlatform(const List<ShaderLanguage>& targetLanguages);
	void CompileLanguage(ShaderLanguage targetLanguage);

	void CompileFolder(sCompileInfo info);
	bool CompileFile(sCompileInfo info);

	bool CompileDefine(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const VecString& defines);
	bool CompileSRGB(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& languageName, const String& define);
	bool CompileImpl(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& languageName, const String& define, bool srgb);

private:
	//获得材质包含的shader
	bool GetMaterialShader(const String& material_file, List<sShaderInfo>& list);
	String GetWriteShaderToFilePath(const String& dirPath, const String& fileName, const String& fileExt, const String& targetVersion, const String& define, const String& languageName, int pass, bool srgb);
	String GetExtensionByShaderStage(ShaderType shader_stage);
	String ShadingLanguageFileName(ShaderLanguage targetLanguage);
	String ReplaceInOut(const String& filePath, const String& content);

private://替换材质
	void ReplaceShaders();
	void ReplaceShader(const String& shaderPath);
	int ReplaceShaderImpl(const String& shaderPath, const String& hlslVersion);
};
DC_END_NAMESPACE
#endif