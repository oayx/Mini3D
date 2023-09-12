#include "DX11Define.h"
#include "core/file/File.h"
#include "runtime/graphics/null/GraphicsDevice.h"
#include "runtime/resources/Resources.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
static String GetCSOFile(const String& name)
{
	String file = name;
	if (file.IndexOf("assets") >= 0)
		file = file.Substring(file.IndexOf("assets") + 7);
	file = file.Replace('.', '_');
	file = file.Replace('/', '_');
	file = file.Replace('\\', '_');
	return file;
}
bool DX11CompileShaderFromFile(const String& file, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer)
{
	DC_PROFILE_FUNCTION();
	if (GetGraphicsCaps()->useProgramBinary)
	{//查看是否有已经编译过的
		String file_name = GetCSOFile(file);
		String file_path = Resource::GetFullSavePath("hlsl/" + file_name + "_" + enter_point + "_" + compile_target + ".cso");
		if (D3DReadFileToBlob(file_path.ToWString().c_str(), codeBuffer) == S_OK)
		{
			return true;
		}
	}

	//没有就重新编译
	Debuger::Log("DX11CompileShaderFromFile:%s, enter_point:%s, target:%s", file.c_str(), enter_point.c_str(), compile_target.c_str());
	String file_content = File::ReadAllText(file);
	return DX11CompileShader(file, file_content, defines, enter_point, compile_target, codeBuffer);
}
bool DX11CompileShader(const String& name, const String& code, const VecString& defines, const String& enter_point, const String& compile_target, ID3DBlob** codeBuffer)
{
	DC_PROFILE_FUNCTION();
	//查看是否有已经编译过的
	String file_path = "";
	if (Application::GetGraphics() && GetGraphicsCaps()->useProgramBinary)
	{
		if (!name.IsEmpty())
		{
			String file_name = GetCSOFile(name);
			file_path = Resource::GetFullSavePath("hlsl/" + file_name + "_" + enter_point + "_" + compile_target + ".cso");
			if (File::Exist(file_path) && D3DReadFileToBlob(file_path.ToWString().c_str(), codeBuffer) == S_OK)
			{
				return true;
			}
		}
	}
	
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DC_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	// 在Debug环境下禁用优化以避免出现一些不合理的情况
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	
	//NOTE:HLSL中#if需要Definition为1，#ifdef只需要定义了Name就会成立
	VecString shader_defineds = defines;
	shader_defineds.Add(ShaderVersion[int(ShaderModel::SM_5_0)]);
	shader_defineds.Add(HLSLDefineVersion[int(ShaderModel::SM_5_0)]);
	shader_defineds.Add("SHADER_API_DIRECTX");
	shader_defineds.Add("UV_STARTS_AT_TOP");
#if defined(DC_COLORSPACE_LINEAR)
	shader_defineds.Add("COLORSPACE_LINEAR");
#endif
	std::unique_ptr<D3D_SHADER_MACRO[]> d3d_defines(DBG_NEW D3D_SHADER_MACRO[shader_defineds.Size() + 1]);
	for (int i = 0; i < shader_defineds.Size(); ++i)
	{
		d3d_defines[i].Name = shader_defineds[i].c_str();
		d3d_defines[i].Definition = "1";
	}
	d3d_defines[shader_defineds.Size()].Name = NULL;
	d3d_defines[shader_defineds.Size()].Definition = NULL;

	ID3DBlob* errorBuffer = NULL;
	HRESULT hr = D3DCompile(code.c_str(), code.Size(), NULL, d3d_defines.get(), D3D_COMPILE_STANDARD_FILE_INCLUDE, enter_point.c_str(), compile_target.c_str(), dwShaderFlags, 0, codeBuffer, &errorBuffer);
	if (errorBuffer != NULL)
	{
		Debuger::Error("DX11CompileShader - D3DCompile : %s", (char*)errorBuffer->GetBufferPointer());
		HR(hr);
		SAFE_RELEASE(errorBuffer);
		return false;
	}
	SAFE_RELEASE(errorBuffer);

	if (Application::GetGraphics() && GetGraphicsCaps()->useProgramBinary)
	{//将着色器二进制信息输出
		if (!file_path.IsEmpty())
		{
			String path = Path::GetDirectoryName(file_path);
			Directory::Create(path);
			HR(D3DWriteBlobToFile(*codeBuffer, file_path.ToWString().c_str(), TRUE));
		}
	}

	return hr == S_OK;
}
DC_END_NAMESPACE