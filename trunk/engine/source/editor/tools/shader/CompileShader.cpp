#include "CompileShader.h"
#include "core/stream/DataStream.h"
#include "runtime/thread/Thread.h"
#include "external/tinyxml2/tinyxml2.h"
#if defined(DC_PLATFORM_WIN32)
#include "runtime/graphics/dx/dx11/DX11ShaderReflect.h"
#include <ShaderConductor/ShaderConductor.hpp>
using namespace ShaderConductor;
#endif
 
DC_BEGIN_NAMESPACE
#if defined(DC_PLATFORM_WIN32)
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
#endif

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
void CompileShader::CompileLanguage(ShaderLanguage target_language)
{
	List<String> target_versions;
	switch (target_language)
	{
	case ShaderLanguage::Dxil:
		//target_versions.Add("5.1");
		target_versions.Add("5.0");
		break;
	case ShaderLanguage::Glsl:
		target_versions.Add("460");
		target_versions.Add("450");
		//target_versions.Add("440");
		//target_versions.Add("430");
		//target_versions.Add("420");
		//target_versions.Add("410");
		//target_versions.Add("400");
		target_versions.Add("330");
		break;
	case ShaderLanguage::Essl:
		target_versions.Add("320");
		//target_versions.Add("310");
		target_versions.Add("300");
		target_versions.Add("100");
		break;
	case ShaderLanguage::SpirV:
		break;
	case ShaderLanguage::Msl_macOS:
		break;
	case ShaderLanguage::Msl_iOS:
		break;
	}

	while (target_versions.Size() > 0)
	{
		sCompileInfo info;
		info.hlsl_version = HLSLVersion;
		info.path = "../../../template/data/assets/internal";
		info.target_language = target_language;
		info.target_version = target_versions.RemoveFirst();
		info.vs_main = Default_VS_Enter;
		info.gs_main = Default_GS_Enter;
		info.ps_main = Default_PS_Enter;
		info.vulkan_text = false;
		info.srgb = true;

		CompileFolder(info);
	}
}
void CompileShader::CompileFolder(sCompileInfo info)
{
	String folder_path = info.path;
	VecString hlsl_files;
	Directory::GetFiles(folder_path, SearchOption::AllDirectories, hlsl_files);
	for (auto file : hlsl_files)
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
	String file_path = info.path;
	String file_name = Path::GetFileNameWithoutExtension(file_path);
	String dir_path = Path::GetDirectoryName(file_path);
	String shader_path = dir_path + "/" + file_name + ".shader";
	List<sShaderInfo> shaders;
	if (File::Exist(shader_path))
	{//判断是否有同名材质
		GetMaterialShader(shader_path, shaders);
	}
	else
	{
		sShaderInfo shader_info;
		shader_info.vs_file = file_path;
		shader_info.gs_file = file_path;
		shader_info.ps_file = file_path;
		shaders.Add(shader_info);
	}

	bool result = true;
	for(auto shader_info : shaders)
	{
		info.pass = shader_info.pass;
		if (!shader_info.vs_enter.IsEmpty()) info.vs_main = shader_info.vs_enter;
		if (!shader_info.gs_enter.IsEmpty()) info.gs_main = shader_info.gs_enter;
		if (!shader_info.ps_enter.IsEmpty()) info.ps_main = shader_info.ps_enter;

		if (!shader_info.vs_file.IsEmpty())
		{
			file_name = Path::GetFileName(shader_info.vs_file);
			if (!CompileDefine(info, ShaderType::Vertex, info.vs_main, shader_info.defines))
				if (result)result = false;
		}

		if (!shader_info.gs_file.IsEmpty())
		{
			file_name = Path::GetFileName(shader_info.gs_file);
			if (!CompileDefine(info, ShaderType::Geometry, info.gs_main, shader_info.defines))
				if (result)result = false;
		}

		if (!shader_info.ps_file.IsEmpty())
		{
			file_name = Path::GetFileName(shader_info.ps_file);
			if (!CompileDefine(info, ShaderType::Pixel, info.ps_main, shader_info.defines))
				if(result)result = false;
		}
	}
	return result;
}
bool CompileShader::CompileDefine(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const VecString& defines)
{
	String language_name = ShadingLanguageFileName(info.target_language);
	bool result = true;
	{//写不带预编译宏的
		bool r = CompileSRGB(info, shader_stage, enter_point, language_name, "");
		if (!r) result = false;
	}
	if (defines.Size() > 0)
	{
		for (int i = 0; i < defines.Size(); ++i)
		{
			const String& define = defines[i];
			if (define.IsEmpty()) continue;

			bool r = CompileSRGB(info, shader_stage, enter_point, language_name, define + "=1");
			if (!r)result = false;
		}
	}
	return result;
}
bool CompileShader::CompileSRGB(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& language_name, const String& define)
{
	bool result = true;
	{
		bool r = CompileImpl(info, shader_stage, enter_point, language_name, define, false);
		if (!r) result = false;
	}
	if (info.srgb)
	{
		bool r = CompileImpl(info, shader_stage, enter_point, language_name, define, true);
		if (!r) result = false;
	}
	return result;
}
bool CompileShader::CompileImpl(sCompileInfo info, ShaderType shader_stage, const String& enter_point, const String& language_name, const String& define, bool srgb)
{
#if defined(DC_PLATFORM_WIN32)
	String source = File::ReadAllText(info.path);

	String defines = define;
	switch (info.target_language)
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
			int version = info.target_version.ToInt();
			if (info.target_language == ShaderLanguage::Essl)
			{
				if (version < 300) defines += ",SM_1_0,SM_2_0";
				else if (version < 320)defines += ",SM_3_0";
				else defines += ",SM_4_0";
			}
			else if (info.target_language == ShaderLanguage::Glsl)
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
				}
				macroDefines.push_back(macroDefine);
			}
		}

		sourceDesc.entryPoint = enter_point.c_str();
		sourceDesc.stage = ConvertToShaderStage(shader_stage);
		sourceDesc.source = source.c_str();
		sourceDesc.fileName = nullptr;
		sourceDesc.defines = macroDefines.data();
		sourceDesc.numDefines = static_cast<uint32_t>(macroDefines.size());

		targetDesc.language = (ShaderConductor::ShadingLanguage)info.target_language;
		targetDesc.version = info.target_version.c_str();

		try
		{
			const auto result = Compiler::Compile(sourceDesc, {}, targetDesc);
			if (result.errorWarningMsg.Size() > 0)
			{
				const char* msg = reinterpret_cast<const char*>(result.errorWarningMsg.Data());
				Debuger::Warning("Error or warning from shader compiler: %s", std::string(msg, msg + result.errorWarningMsg.Size()).c_str());
			}
			if (result.target.Size() > 0)
			{
				String outputName = GetWriteShaderToFilePath(Path::GetDirectoryName(info.path), Path::GetFileNameWithoutExtension(info.path), GetExtensionByShaderStage(shader_stage), info.target_version, define, language_name, info.pass, srgb);
				std::ofstream outputFile(outputName.c_str(), std::ios_base::binary);
				if (!outputFile)
				{
					Debuger::Error("Cannot open the output file:%s", outputName.c_str());
					return false;
				}

				outputFile.write(reinterpret_cast<const char*>(result.target.Data()), result.target.Size());
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
#else
	return false;
#endif
}

bool CompileShader::GetMaterialShader(const String& material_file, List<sShaderInfo>& list)
{
	tinyxml2::XMLDocument doc;
	FileDataStream open_stream(material_file, "rb");
	tinyxml2::XMLError error = doc.LoadFile(open_stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("open material file error1:%s", material_file.c_str());
		return false;
	}

	int passIdx = 0;
	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* pass_node = root->FirstChildElement("Pass");
	while (pass_node != nullptr)
	{
		tinyxml2::XMLElement* shader_node = pass_node->FirstChildElement("CG");
		if (shader_node != nullptr)
		{
			sShaderInfo shader_info;
			shader_info.pass = passIdx;

			String shader_file = "";
			if (shader_node->Attribute("File") != nullptr)
				shader_file = shader_node->Attribute("File");

			tinyxml2::XMLElement* shader_child_node = shader_node->FirstChildElement("VS");
			if (shader_child_node != nullptr)
			{
				shader_info.vs_file = shader_file;
				shader_info.vs_enter = shader_child_node->Attribute("Enter");
				shader_info.vs_version = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("HS");
			if (shader_child_node != nullptr)
			{
				shader_info.hs_file = shader_file;
				shader_info.hs_enter = shader_child_node->Attribute("Enter");
				shader_info.hs_version = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("DS");
			if (shader_child_node != nullptr)
			{
				shader_info.ds_file = shader_file;
				shader_info.ds_enter = shader_child_node->Attribute("Enter");
				shader_info.ds_version = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("GS");
			if (shader_child_node != nullptr)
			{
				shader_info.gs_file = shader_file;
				shader_info.gs_enter = shader_child_node->Attribute("Enter");
				shader_info.gs_version = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("PS");
			if (shader_child_node != nullptr)
			{
				shader_info.ps_file = shader_file;
				shader_info.ps_enter = shader_child_node->Attribute("Enter");
				shader_info.ps_version = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("CS");
			if (shader_child_node != nullptr)
			{
				shader_info.cs_file = shader_file;
				shader_info.cs_enter = shader_child_node->Attribute("Enter");
				shader_info.cs_version = shader_child_node->Attribute("Target");
			}
			//TODO:未测试
			shader_child_node = shader_node->FirstChildElement("Defines");
			if (shader_child_node != nullptr)
			{
				String name = shader_child_node->Attribute("Name");
				VecString defines = name.Split(",");
				for (auto define : defines)
				{
					shader_info.defines.Add(define);
				}
			}
			list.Add(shader_info);
		}

		passIdx++;
		pass_node = pass_node->NextSiblingElement("Pass");
	}
	return true;
}
String CompileShader::GetWriteShaderToFilePath(const String& dir_path, const String& file_name, const String& file_ext, const String& target_version, const String& define, const String& language_name, int pass, bool srgb)
{
	String shader_file_path = dir_path + "/" + file_name + "_" + language_name;
	if (!target_version.IsEmpty())
		shader_file_path += "_" + target_version;
	if (!define.IsEmpty())
		shader_file_path += "_" + define;
	if (pass > 0) shader_file_path += "_pass" + String::ToString(pass);
	if (srgb) shader_file_path += "_srgb";
	shader_file_path += file_ext;
	return shader_file_path;
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
String CompileShader::ShadingLanguageFileName(ShaderLanguage target_language)
{
	switch (target_language)
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
void CompileShader::ReplaceShaders()
{
	VecString hlsl_files;
	Directory::GetFiles("../../../template/data/assets/internal", SearchOption::AllDirectories, hlsl_files);
	for (auto file : hlsl_files)
	{
		if (file.EndsWith(".shader"))
		{
			ReplaceShader(file);
		}
	}
}
void CompileShader::ReplaceShader(const String& shader_path)
{
	//替换材质
	if (File::Exist(shader_path))
	{
		int result_code = ReplaceShaderImpl(shader_path, HLSLVersion);
		if (result_code != 0)
		{
			Debuger::Error("替换材质失败,file:%s,result:%d", shader_path.c_str(), result_code);
		}
	}
}
int CompileShader::ReplaceShaderImpl(const String& shader_path, const String& hlsl_version)
{
	String material_file(shader_path);
	material_file = material_file.Replace('\\', '/');
	String asset_path = Path::GetDirectoryName(material_file);

	tinyxml2::XMLDocument doc;
	FileDataStream open_stream(material_file, "rb+");
	tinyxml2::XMLError error = doc.LoadFile(open_stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
		return 1;

	int result = 0;
	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* pass_node = root->FirstChildElement("Pass");
	while (pass_node != nullptr)
	{
		tinyxml2::XMLElement* shader_node = pass_node->FirstChildElement("CG");
		if (shader_node != nullptr)
		{
			String shader_file = "";
			if (shader_node->Attribute("File") != nullptr)
				shader_file = shader_node->Attribute("File");

			ShaderDesc shader_info;
			tinyxml2::XMLElement* shader_child_node = shader_node->FirstChildElement("VS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Vertex)] = shader_file;
				shader_info.Enter[int(ShaderType::Vertex)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Vertex)] = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("HS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Hull)] = shader_file;
				shader_info.Enter[int(ShaderType::Hull)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Hull)] = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("DS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Domain)] = shader_file;
				shader_info.Enter[int(ShaderType::Domain)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Domain)] = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("GS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Geometry)] = shader_file;
				shader_info.Enter[int(ShaderType::Geometry)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Geometry)] = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("PS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Pixel)] = shader_file;
				shader_info.Enter[int(ShaderType::Pixel)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Pixel)] = shader_child_node->Attribute("Target");
			}
			shader_child_node = shader_node->FirstChildElement("CS");
			if (shader_child_node != nullptr)
			{
				shader_info.ShaderFile[int(ShaderType::Compute)] = shader_file;
				shader_info.Enter[int(ShaderType::Compute)] = shader_child_node->Attribute("Enter");
				shader_info.Target[int(ShaderType::Compute)] = shader_child_node->Attribute("Target");
			}
#if defined(DC_PLATFORM_WIN32)
			DX11ShaderReflect reflect;
			result = reflect.Reflect(asset_path, shader_info, hlsl_version.c_str());
			if (result == 0)
			{
				//删除已有的
				tinyxml2::XMLElement* reflect_node = shader_node->FirstChildElement("Reflect");
				while (reflect_node != nullptr)
				{
					shader_node->DeleteChild(reflect_node);
					reflect_node = shader_node->FirstChildElement("Reflect");
				}

				//插入新的
				{
					reflect_node = shader_node->InsertNewChildElement("Reflect");
					//输入语义
					tinyxml2::XMLElement* semantics_node = reflect_node->InsertNewChildElement("Semantics");
					semantics_node->SetAttribute("Value", (uint)reflect.mVertexSemantic);
					//CBuffer
					if (!reflect.mCBBuffers.IsEmpty())
					{
						tinyxml2::XMLElement* cbuffers_node = reflect_node->InsertNewChildElement("CBuffers");
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
						tinyxml2::XMLElement* textures_node = reflect_node->InsertNewChildElement("Textures");
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
#endif
		}

		pass_node = pass_node->NextSiblingElement("Pass");
	}

	FileDataStream save_stream(material_file, "wb+");
	error = doc.SaveFile(save_stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
		return 3;
	return result;
}
DC_END_NAMESPACE