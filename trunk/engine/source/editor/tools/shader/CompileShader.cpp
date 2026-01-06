#if defined(DC_PLATFORM_WIN32) && defined(_WIN64)
#include "CompileShader.h"
#include "core/stream/DataStream.h"
#include "runtime/graphics/ShaderlabUtils.h"
#include "runtime/thread/Thread.h"
#include "external/tinyxml2/tinyxml2.h"
#include "external/shaderlab/ShaderParser.h"
#include "runtime/graphics/dx/dx11/DX11ShaderReflect.h"
#include <ShaderConductor/ShaderConductor.hpp>

using namespace ShaderConductor;
using namespace shaderlab;
 
DC_BEGIN_NAMESPACE
static ShaderConductor::ShaderStage ConvertToShaderStage(ShaderType type)
{
	switch (type)
	{
	case ShaderType::Vertex:return ShaderConductor::ShaderStage::VertexShader;
	case ShaderType::Hull:return ShaderConductor::ShaderStage::HullShader;
	case ShaderType::Domain:return ShaderConductor::ShaderStage::DomainShader;
	case ShaderType::Geometry:return ShaderConductor::ShaderStage::GeometryShader;
	case ShaderType::Pixel:return ShaderConductor::ShaderStage::PixelShader;
	case ShaderType::Compute:return ShaderConductor::ShaderStage::ComputeShader;
	default: AssertEx(0, "%d", type); return ShaderConductor::ShaderStage::NumShaderStages;
	}
}

/********************************************************************/
void CompileShader::StartCompile()
{
	List<ShaderLanguage> targetLanguages;
	targetLanguages.Add(ShaderLanguage::Glsl);
	targetLanguages.Add(ShaderLanguage::Essl);
	//targetLanguages.Add(ShaderLanguage::Dxil);
	//targetLanguages.Add(ShaderLanguage::SpirV);
	//targetLanguages.Add(ShaderLanguage::Msl_macOS);
	//targetLanguages.Add(ShaderLanguage::Msl_iOS);
	CompilePlatform(targetLanguages);
}
void CompileShader::CompilePlatform(const List<ShaderLanguage>& targetLanguages)
{
	for (auto language : targetLanguages)
	{
		CompileLanguage(language);
	}

	Debuger::Log("编译结束");
	Debuger::Log("----------替换shader文件----------");
	ReplaceShaders();
	Debuger::Log("替换完成");
}
void CompileShader::CompileLanguage(ShaderLanguage targetLanguage)
{
	List<String> targetVersions;
	switch (targetLanguage)
	{
	case ShaderLanguage::Dxil:
		//targetVersions.Add("5.1");
		targetVersions.Add("5.0");
		break;
	case ShaderLanguage::Glsl:
		targetVersions.Add("460");
		targetVersions.Add("450");
		//targetVersions.Add("440");
		//targetVersions.Add("430");
		//targetVersions.Add("420");
		targetVersions.Add("410");
		//targetVersions.Add("400");
		targetVersions.Add("330");
		break;
	case ShaderLanguage::Essl:
		targetVersions.Add("320");
		//targetVersions.Add("310");
		targetVersions.Add("300");
		targetVersions.Add("100");
		break;
	case ShaderLanguage::SpirV:
		break;
	case ShaderLanguage::Msl_macOS:
		break;
	case ShaderLanguage::Msl_iOS:
		break;
	}

	while (targetVersions.Size() > 0)
	{
		sCompileInfo info;
		info.hlslVersion = HLSLVersion;
		info.path = "../../../template/data/assets/internal";
		info.targetLanguage = targetLanguage;
		info.targetVersion = targetVersions.RemoveFirst();
		info.vs_main = Default_VS_Enter;
		info.gs_main = Default_GS_Enter;
		info.ps_main = Default_PS_Enter;
		info.ds_main = Default_DS_Enter;
		info.hs_main = Default_HS_Enter;
		info.cs_main = Default_CS_Enter;
		info.isVulkanText = false;
		info.srgb = true;

		CompileFolder(info);
	}
}
void CompileShader::CompileFolder(sCompileInfo info)
{
	String folderPath = info.path;
	VecString hlslFiles;
	Directory::GetFiles(folderPath, SearchOption::AllDirectories, hlslFiles);
	for (auto file : hlslFiles)
	{
		if (file.EndsWith(".hlsl"))
		{
			info.path = file;
			CompileFile(info);
		}
	}
}
bool CompileShader::CompileFile(sCompileInfo info)
{
	String filePath = info.path;
	String fileName = Path::GetFileNameWithoutExtension(filePath);
	String dirPath = Path::GetDirectoryName(filePath);
	String shaderPath = dirPath + "/" + fileName + ".shader";
	List<sShaderInfo> shaders;
	if (File::Exist(shaderPath))
	{//判断是否有同名材质
		GetMaterialShader(shaderPath, shaders);
	}
	else
	{
		sShaderInfo shaderInfo;
		shaderInfo.vsFile = filePath;
		shaderInfo.gsFile = filePath;
		shaderInfo.psFile = filePath;
		shaders.Add(shaderInfo);
	}

	bool result = true;
	for(auto shaderInfo : shaders)
	{
		info.pass = shaderInfo.pass;
		if (!shaderInfo.vsEnter.IsEmpty()) info.vs_main = shaderInfo.vsEnter;
		if (!shaderInfo.gsEnter.IsEmpty()) info.gs_main = shaderInfo.gsEnter;
		if (!shaderInfo.psEnter.IsEmpty()) info.ps_main = shaderInfo.psEnter;

		if (!shaderInfo.vsFile.IsEmpty())
		{
			fileName = Path::GetFileName(shaderInfo.vsFile);
			if (!CompileDefine(info, ShaderType::Vertex, info.vs_main, shaderInfo.defines))
				if (result)result = false;
		}

		if (!shaderInfo.gsFile.IsEmpty())
		{
			fileName = Path::GetFileName(shaderInfo.gsFile);
			if (!CompileDefine(info, ShaderType::Geometry, info.gs_main, shaderInfo.defines))
				if (result)result = false;
		}

		if (!shaderInfo.psFile.IsEmpty())
		{
			fileName = Path::GetFileName(shaderInfo.psFile);
			if (!CompileDefine(info, ShaderType::Pixel, info.ps_main, shaderInfo.defines))
				if(result)result = false;
		}
	}
	return result;
}
bool CompileShader::CompileDefine(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const VecString& defines)
{
	String languageName = ShadingLanguageFileName(info.targetLanguage);
	bool result = true;
	{//写不带预编译宏的
		bool r = CompileSRGB(info, shader_stage, enter_point, languageName, "");
		if (!r) result = false;
	}
	if (defines.Size() > 0)
	{
		for (int i = 0; i < defines.Size(); ++i)
		{
			const String& define = defines[i];
			if (define.IsEmpty()) continue;

			bool r = CompileSRGB(info, shader_stage, enter_point, languageName, define);
			if (!r)result = false;
		}
	}
	return result;
}
bool CompileShader::CompileSRGB(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& languageName, const String& define)
{
	bool result = true;
	{
		bool r = CompileImpl(info, shader_stage, enter_point, languageName, define, false);
		if (!r) result = false;
	}
	if (info.srgb)
	{
		bool r = CompileImpl(info, shader_stage, enter_point, languageName, define, true);
		if (!r) result = false;
	}
	return result;
}
bool CompileShader::CompileImpl(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& languageName, const String& define, bool srgb)
{
	String source = File::ReadAllText(info.path);

	String defines = define;
	switch (info.targetLanguage)
	{
		case ShaderLanguage::Dxil:
		{
			if (define.IsEmpty())
				defines = "SHADER_API_DIRECTX=1,UV_STARTS_AT_TOP=1";
			else
				defines += ",SHADER_API_DIRECTX=1,UV_STARTS_AT_TOP=1";
			break;
		}
		case ShaderLanguage::Glsl:
		case ShaderLanguage::Essl:
		{
			if (define.IsEmpty())
				defines = "SHADER_API_OPENGL=1";
			else
				defines += ",SHADER_API_OPENGL=1";
			int version = info.targetVersion.ToInt();
			if (info.targetLanguage == ShaderLanguage::Essl)
			{
				if (version < 300) defines += ",SM_1_0,SM_2_0";
				else if (version < 320)defines += ",SM_3_0";
				else defines += ",SM_4_0";
			}
			else if (info.targetLanguage == ShaderLanguage::Glsl)
			{
				if (version < 110) defines += ",SM_1_0";
				else if (version < 130) defines += ",SM_2_0";
				else if (version < 330) defines += ",SM_3_0";
				else if (version < 430) defines += ",SM_4_0";
				else defines += ",SM_5_0";
			}
			break;
		}
		case ShaderLanguage::SpirV:
		{
			if (define.IsEmpty())
				defines = "SHADER_API_VULKAN=1";
			else
				defines += ",SHADER_API_VULKAN=1";
			defines += ",HLSL_VERSION_5_1=1";
			break;
		}
	}
	if (srgb)
	{
		defines += ",COLORSPACE_LINEAR=1";
	}

	{//编译
		Compiler::SourceDesc sourceDesc{};
		Compiler::TargetDesc targetDesc{};

		std::vector<MacroDefine> macroDefines;
		if (!defines.IsEmpty())
		{
			size_t len = 0;
			Vector<String> str_arrs = defines.Split(",");
			for (int i = 0; i < str_arrs.Size(); ++i)
			{
				MacroDefine macroDefine;

				std::string key_values = str_arrs[i];
				size_t splitPosition = key_values.find('=');
				if (splitPosition != std::string::npos)
				{
					std::string macroName = key_values.substr(0, splitPosition);
					std::string macroValue = key_values.substr(splitPosition + 1, key_values.size() - splitPosition - 1);

					memset(macroDefine.name, 0, sizeof(macroDefine.name));
					strcpy_s(macroDefine.name, macroName.length() + 1, macroName.data());
					memset(macroDefine.value, 0, sizeof(macroDefine.value));
					strcpy_s(macroDefine.value, macroValue.length() + 1, macroValue.data());
					len = macroName.length();
				}
				else
				{
					memset(macroDefine.name, 0, sizeof(macroDefine.name));
					strcpy_s(macroDefine.name, key_values.length() + 1, key_values.data());
					strcpy_s(macroDefine.value, 2, "1");
				}
				macroDefines.push_back(macroDefine);
			}
		}

		String fileName = Path::GetFileName(info.path);
		sourceDesc.entryPoint = enter_point.c_str();
		sourceDesc.stage = ConvertToShaderStage(shader_stage);
		sourceDesc.source = source.c_str();
		sourceDesc.fileName = fileName.c_str();
		sourceDesc.defines = macroDefines.data();
		sourceDesc.numDefines = static_cast<uint32_t>(macroDefines.size());

		targetDesc.language = (ShaderConductor::ShadingLanguage)info.targetLanguage;
		targetDesc.version = info.targetVersion.c_str();

		try
		{
			const auto result = Compiler::Compile(sourceDesc, {}, targetDesc);
			if (result.errorWarningMsg.Size() > 0)
			{
				const char* msg = reinterpret_cast<const char*>(result.errorWarningMsg.Data());
				Debuger::Warning("compiler shader file: %s, msg: %s",info.path.c_str(), std::string(msg, msg + result.errorWarningMsg.Size()).c_str());
			}
			if (result.target.Size() > 0)
			{
				String outputName = GetWriteShaderToFilePath(Path::GetDirectoryName(info.path), Path::GetFileNameWithoutExtension(info.path), GetExtensionByShaderStage(shader_stage), info.targetVersion, define, languageName, info.pass, srgb);
				std::ofstream outputFile(outputName.c_str(), std::ios_base::binary);
				if (!outputFile)
				{
					Debuger::Error("Cannot open the output file:%s", outputName.c_str());
					return false;
				}

				//替换out_和in_，mac下需要vs和ps互通的变量命名一样
				String content(reinterpret_cast<const char*>(result.target.Data()), result.target.Size());
				content = ReplaceInOut(outputName, content);

				outputFile.write(content.data(), content.Size());
			}
			else
			{
				Debuger::Error("Cannot write file:%s", info.path.c_str());
			}
		}
		catch (std::exception& ex)
		{
			Debuger::Error("Compile file:%s, error:%s", info.path.c_str(), ex.what());
			return false;
		}
	}
	return true;
}

bool CompileShader::GetMaterialShader(const String& shaderPath, List<sShaderInfo>& list)
{
	const std::shared_ptr<ASTNode>& root = ShaderlabUtils::ParseShader(shaderPath);
	if (root == nullptr)
		return false;
	int passIdx = 0;
	for (const auto& node : root->children)
	{
		if (node->Type == ASTNodeType::SubShader)
		{
			for (const auto& subShaderNode : node->children)
			{
				if (subShaderNode->Type == ASTNodeType::Pass)
				{
					for (const auto& passNode : subShaderNode->children)
					{
						if (passNode->Type == ASTNodeType::ShaderBlock)
						{
							const auto& node = std::dynamic_pointer_cast<ASTShaderBlockNode>(passNode);
							CGProgramData data = ShaderlabUtils::ParseCG(node->ShaderCode);
							if (data.Includes.size() > 0)
							{
								sShaderInfo shaderInfo;
								shaderInfo.pass = passIdx++;

								String shaderFile = data.Includes[0];
								for (auto& define : data.Defines)
								{
									shaderInfo.defines.Add(define);
								}
								for (auto& enter : data.Pragmas)
								{
									String type = enter.first;
									if (type.Equals("vertex", true))
									{
										shaderInfo.vsFile = shaderFile;
										shaderInfo.vsEnter = enter.second;
									}
									else if (type.Equals("fragment", true))
									{
										shaderInfo.psFile = shaderFile;
										shaderInfo.psEnter = enter.second;
									}
									else if (type.Equals("geometry", true))
									{
										shaderInfo.gsFile = shaderFile;
										shaderInfo.gsEnter = enter.second;
									}
									else if (type.Equals("hull", true))
									{
										shaderInfo.hsFile = shaderFile;
										shaderInfo.hsEnter = enter.second;
									}
									else if (type.Equals("domain", true))
									{
										shaderInfo.dsFile = shaderFile;
										shaderInfo.dsEnter = enter.second;
									}
									else if (type.Equals("compute", true))
									{
										shaderInfo.csFile = shaderFile;
										shaderInfo.csEnter = enter.second;
									}
								}

								list.Add(shaderInfo);
							}
						}
					}
				}
			}
		}
	}
	return true;
}
String CompileShader::GetWriteShaderToFilePath(const String& dirPath, const String& fileName, const String& fileExt, const String& targetVersion, const String& define, const String& languageName, int pass, bool srgb)
{
	String shaderFilePath = dirPath + "/" + fileName + "_" + languageName;
	if (!targetVersion.IsEmpty())
		shaderFilePath += "_" + targetVersion;
	if (!define.IsEmpty())
		shaderFilePath += "_" + define;
	if (pass > 0) shaderFilePath += "_pass" + String::ToString(pass);
	if (srgb) shaderFilePath += "_srgb";
	shaderFilePath += fileExt;
	return shaderFilePath;
}
String CompileShader::GetExtensionByShaderStage(ShaderType shader_stage)
{
	switch (shader_stage)
	{
	case ShaderType::Vertex:return ".vs";
	case ShaderType::Hull: return ".hs";
	case ShaderType::Domain: return ".ds";
	case ShaderType::Geometry: return ".gs";
	case ShaderType::Pixel:return ".ps";
	case ShaderType::Compute: return ".cs";
	default: return "";
	}
}
String CompileShader::ShadingLanguageFileName(ShaderLanguage targetLanguage)
{
	switch (targetLanguage)
	{
	case ShaderLanguage::Dxil:return "dxil";
	case ShaderLanguage::SpirV: return "spirV";
	case ShaderLanguage::Hlsl: return "hlsl";
	case ShaderLanguage::Glsl: return "glsl";
	case ShaderLanguage::Essl: return "essl";
	case ShaderLanguage::Msl_macOS: return "metal_mac";
	case ShaderLanguage::Msl_iOS: return "metal_ios";
	default:return "";
	}
}
String CompileShader::ReplaceInOut(const String& filePath, const String& content)
{
	String outContent;
	String ext = Path::GetExtension(filePath);
	if (ext == ".vs")
	{
		outContent = content.Replace("out_", "");
	}
	else if (ext == ".ps")
	{
		outContent = content.Replace("in_", "");
	}
	else if (ext == ".gs")
	{
		outContent = content.Replace("out_", "");
		outContent = outContent.Replace("in_", "");
	}
	return outContent;
}
void CompileShader::ReplaceShaders()
{
	VecString hlslFiles;
	Directory::GetFiles("../../../template/data/assets/internal", SearchOption::AllDirectories, hlslFiles);
	for (auto file : hlslFiles)
	{
		if (file.EndsWith(".shader") && !file.EndsWith("1.shader"))
		{
			ReplaceShader(file);
		}
	}
}
void CompileShader::ReplaceShader(const String& shaderPath)
{
	//替换材质
	if (File::Exist(shaderPath))
	{
		int resultCode = ReplaceShaderImpl(shaderPath, HLSLVersion);
		if (resultCode != 0)
		{
			Debuger::Error("replace shader error,file:%s,result:%d", shaderPath.c_str(), resultCode);
		}
	}
}
int CompileShader::ReplaceShaderImpl(const String& shaderPath, const String& hlslVersion)
{
	String shaderFullPath = shaderPath.Replace('\\', '/');
	String assetPath = Path::GetDirectoryName(shaderFullPath);

	//读取shader cg info
	std::shared_ptr<ASTNode> astRoot = ShaderlabUtils::ParseShader(shaderFullPath);
	if (astRoot == nullptr)
	{
		Debuger::Error("Parse shaderlab file error:%s", shaderPath.c_str());
		return 1;
	}
	Vector<std::shared_ptr<ASTNode>> astShaderBlockNodes;
	for (const auto& astChildNode : astRoot->children)
	{
		if (astChildNode->Type == ASTNodeType::SubShader)
		{
			for (const auto& astSubShaderNode : astChildNode->children)
			{
				if (astSubShaderNode->Type == ASTNodeType::Pass)
				{
					for (const auto& astPassNode : astSubShaderNode->children)
					{
						if (astPassNode->Type == ASTNodeType::ShaderBlock)
						{
							astShaderBlockNodes.Add(astPassNode);
						}
					}
				}
			}
		}
	}

	//通过编译shader，反射出信息后，写入xml文件
	String cgFullPath = shaderFullPath.Replace(".shader", ".cache");
	if (!File::Exist(cgFullPath))
	{
		tinyxml2::XMLDocument docNew;
		tinyxml2::XMLElement* shaderNode = docNew.NewElement("Shader");
		docNew.InsertEndChild(shaderNode);
		tinyxml2::XMLElement* passNode = docNew.NewElement("Pass");
		shaderNode->InsertEndChild(passNode);
		tinyxml2::XMLElement* cgNode = docNew.NewElement("CG");
		passNode->InsertEndChild(cgNode);
		tinyxml2::XMLError err = docNew.SaveFile(cgFullPath.c_str());
		if (err != tinyxml2::XML_SUCCESS)
		{
			Debuger::Error("Create new xml error:%s", cgFullPath.c_str());
			return 2;
		}
	}

	tinyxml2::XMLDocument doc;
	FileDataStream openStream(cgFullPath, "rb+");
	tinyxml2::XMLError error = doc.LoadFile(openStream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("Open xml error:%s", cgFullPath.c_str());
		return 3;
	}

	int result = 0;
	int passIdx = 0;
	tinyxml2::XMLElement* xmlRoot = doc.RootElement();
	tinyxml2::XMLElement* passNode = xmlRoot->FirstChildElement("Pass");
	while (passNode != nullptr)
	{
		tinyxml2::XMLElement* shaderNode = passNode->FirstChildElement("CG");
		if (shaderNode != nullptr)
		{
			ShaderDesc shaderInfo;
			const auto& astShaderNode = std::dynamic_pointer_cast<ASTShaderBlockNode>(astShaderBlockNodes[passIdx]);
			CGProgramData data = ShaderlabUtils::ParseCG(astShaderNode->ShaderCode);
			if (data.Includes.size() > 0)
			{
				String shaderFile = data.Includes[0];
				for (auto& define : data.Defines)
				{
					shaderInfo.ShaderDefines.Add(define);
				}
				for (auto& enter : data.Pragmas)
				{
					String type = enter.first;
					if (type.Equals("vertex", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Vertex)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Vertex)] = enter.second;
					}
					else if (type.Equals("fragment", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Pixel)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Pixel)] = enter.second;
					}
					else if (type.Equals("geometry", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Geometry)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Geometry)] = enter.second;
					}
					else if (type.Equals("hull", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Hull)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Hull)] = enter.second;
					}
					else if (type.Equals("domain", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Domain)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Domain)] = enter.second;
					}
					else if (type.Equals("compute", true))
					{
						shaderInfo.ShaderFile[int(ShaderType::Compute)] = shaderFile;
						shaderInfo.Enter[int(ShaderType::Compute)] = enter.second;
					}
				}
			}

			DX11ShaderReflect reflect;
			result = reflect.Reflect(assetPath, shaderInfo, hlslVersion.c_str());
			if (result == 0)
			{
				//删除已有的
				tinyxml2::XMLElement* reflectNode = shaderNode->FirstChildElement("Reflect");
				while (reflectNode != nullptr)
				{
					shaderNode->DeleteChild(reflectNode);
					reflectNode = shaderNode->FirstChildElement("Reflect");
				}

				//插入新的
				{
					reflectNode = shaderNode->InsertNewChildElement("Reflect");
					//输入语义
					tinyxml2::XMLElement* semantics_node = reflectNode->InsertNewChildElement("Semantics");
					semantics_node->SetAttribute("Value", (uint)reflect.mVertexSemantic);
					//CBuffer
					if (!reflect.mCBBuffers.IsEmpty())
					{
						tinyxml2::XMLElement* cbuffers_node = reflectNode->InsertNewChildElement("CBuffers");
						for (const auto& obj : reflect.mCBBuffers)
						{
							const sShaderReflectCBuffer& cbuffer_info = obj.second;
							tinyxml2::XMLElement* cbuffer_node = cbuffers_node->InsertNewChildElement("CBuffer");
							cbuffer_node->SetAttribute("Name", cbuffer_info.name.c_str());
							cbuffer_node->SetAttribute("Slot", (int)cbuffer_info.slot);
							cbuffer_node->SetAttribute("Size", cbuffer_info.size);
							cbuffer_node->SetAttribute("Stage", cbuffer_info.stage);

							//变量
							if (!reflect.mCBVariables.IsEmpty())
							{
								tinyxml2::XMLElement* variables_node = cbuffer_node->InsertNewChildElement("Variables");
								for (const auto& obj : reflect.mCBVariables)
								{
									const sShaderReflectVariable& variable_info = obj.second;
									if (variable_info.cb_name != cbuffer_info.name)continue;

									tinyxml2::XMLElement* variable_node = variables_node->InsertNewChildElement("Variable");
									variable_node->SetAttribute("Name", variable_info.var_name.c_str());
									variable_node->SetAttribute("Offset", variable_info.offset);
									variable_node->SetAttribute("Size", variable_info.size);
									variable_node->SetAttribute("Stage", (uint)variable_info.stage);
								}
							}
						}
					}
					//纹理
					if (!reflect.mTextures.IsEmpty())
					{
						tinyxml2::XMLElement* textures_node = reflectNode->InsertNewChildElement("Textures");
						for (const auto& obj : reflect.mTextures)
						{
							const sShaderReflectTexture& texture_info = obj.second;
							tinyxml2::XMLElement* texture_node = textures_node->InsertNewChildElement("Texture");
							texture_node->SetAttribute("Name", texture_info.name.c_str());
							texture_node->SetAttribute("Type", (int)texture_info.type);
							texture_node->SetAttribute("Slot", texture_info.slot);
							texture_node->SetAttribute("Stage", texture_info.stage);
						}
					}
				}
			}
		}
		passIdx++;
		passNode = passNode->NextSiblingElement("Pass");
	}

	FileDataStream save_stream(cgFullPath, "wb+");
	error = doc.SaveFile(save_stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("Save xml error:%s", cgFullPath.c_str());
		return 4;
	}
	return result;
}
DC_END_NAMESPACE
#endif