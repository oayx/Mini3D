#include "DX11ShaderReflect.h"
#include "core/file/File.h"
#include "core/stream/DataStream.h"
#include "runtime/input/Input.h"
#include "runtime/Application.h"
#include "external/tinyxml2/tinyxml2.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DX11ShaderReflect, Object);
void DX11ShaderReflect::Reflect(ShaderType type, ID3DBlob* shader_code)
{
	//反射shader信息
	ID3D11ShaderReflection* reflector = nullptr;
	DX_ERROR(D3DReflect(shader_code->GetBufferPointer(), shader_code->GetBufferSize(), __uuidof(ID3D11ShaderReflection), (void**)&reflector));
	
	D3D11_SHADER_DESC shader_desc;
	DX_ERROR(reflector->GetDesc(&shader_desc));
	//Debuger::Log("shader version:%u", shader_desc.Version);
	for (int i = 0; i != shader_desc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC bind_desc;
		DX_ERROR(reflector->GetResourceBindingDesc(i, &bind_desc));
		switch (bind_desc.Type)
		{
		case D3D_SIT_CBUFFER:
		{
			ID3D11ShaderReflectionConstantBuffer *cb = reflector->GetConstantBufferByName(bind_desc.Name);
			D3D11_SHADER_BUFFER_DESC cb_desc;
			DX_ERROR(cb->GetDesc(&cb_desc));

			auto it_cbuffer = mCBBuffers.Find(bind_desc.Name);
			if (it_cbuffer == mCBBuffers.end())
			{
				sShaderReflectCBuffer buffer;
				buffer.slot = bind_desc.BindPoint;
				buffer.name = bind_desc.Name;
				buffer.size = cb_desc.Size;
				buffer.stage = (uint)ShaderStageEnum[(int)type];
				AssertEx(mCBBuffers.Add(buffer.name, buffer), "");
			}
			else
			{
				it_cbuffer->second.stage |= (uint)ShaderStageEnum[(int)type];
			}

			//获得cb所有变量信息
			for (uint j = 0; j < cb_desc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* c_var = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC var_desc;
				c_var->GetDesc(&var_desc);

				auto it = mCBVariables.Find(var_desc.Name);
				if (it == mCBVariables.end())
				{
					sShaderReflectVariable cb_variable;
					cb_variable.cb_name = bind_desc.Name;
					cb_variable.var_name = var_desc.Name;
					cb_variable.offset = var_desc.StartOffset;
					cb_variable.size = var_desc.Size;
					cb_variable.stage = ShaderStageEnum[(int)type];
					AssertEx(mCBVariables.Add(cb_variable.var_name, cb_variable), "");
				}
				else
				{
					it->second.stage |= ShaderStageEnum[(int)type];
				}
			}
		}
		break;
		case D3D_SIT_TEXTURE:
		{
			auto it = mTextures.Find(bind_desc.Name);
			if (it == mTextures.end())
			{
				sShaderReflectTexture texture;
				texture.name = bind_desc.Name;
				texture.slot = bind_desc.BindPoint;
				texture.stage = (uint)ShaderStageEnum[(int)type];
				switch (bind_desc.Dimension)
				{
				case D3D_SRV_DIMENSION_TEXTURE2D:		texture.type = TextureType::D2; break;
				case D3D_SRV_DIMENSION_TEXTURE3D:		texture.type = TextureType::D3; break;
				case D3D_SRV_DIMENSION_TEXTURECUBE:		texture.type = TextureType::Cube; break;
				case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	texture.type = TextureType::D2Array; break;
				}
				AssertEx(mTextures.Add(texture.name ,texture), "");
			}
			else
			{
				it->second.stage |= (uint)ShaderStageEnum[(int)type];
			}


		}
		break;
		case D3D_SIT_SAMPLER:
		{

		}
		break;
		}
	}

	for (int i = 0; i != shader_desc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer *cb = reflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cb_desc;
		DX_ERROR(cb->GetDesc(&cb_desc));

		ShaderReflectCBuffers::const_iterator it = mCBBuffers.Find(cb_desc.Name);
		if (it == mCBBuffers.end())
		{
			Debuger::Warning("cannot find buffer:%s", cb_desc.Name);
			continue;
		}
		const sShaderReflectCBuffer& buffer = it->second;
		for (uint j = 0; j < cb_desc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* c_var = cb->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC var_desc;
			DX_ERROR(c_var->GetDesc(&var_desc));

			auto it = mCBVariables.Find(var_desc.Name);
			if (it == mCBVariables.end())
			{
				sShaderReflectVariable cb_variable;
				cb_variable.cb_name = buffer.name;
				cb_variable.var_name = var_desc.Name;
				cb_variable.offset = var_desc.StartOffset;
				cb_variable.size = var_desc.Size;
				cb_variable.stage = ShaderStageEnum[(int)type];
				AssertEx(mCBVariables.Add(cb_variable.var_name, cb_variable), "");
			}
			else
			{
				it->second.stage |= ShaderStageEnum[(int)type];
			}
		}
	}

	//获取输入语义
	if (type == ShaderType::Vertex)
	{
		for (int i = 0; i != shader_desc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC input_desc;
			DX_ERROR(reflector->GetInputParameterDesc(i, &input_desc));
			VertexSemantic sem = DXGetVertexSemantic(input_desc.SemanticIndex, input_desc.SemanticName);
			mVertexSemantic |= sem;
		}
	}
	SAFE_RELEASE(reflector);
}
int DX11ShaderReflect::Reflect(const String& path, const ShaderDesc& shaderInfo, const char* version)
{
	for (int i = 0; i < int(ShaderType::Max); ++i)
	{
		if (shaderInfo.ShaderFile[i].IsEmpty())continue;

		ShaderType shader_type = ShaderType(i);
		String content = File::ReadAllText(Path::Combine(path, shaderInfo.ShaderFile[i]));
		if (!content.IsEmpty())
		{
			String enter_point = shaderInfo.Enter[i];
			if (enter_point.IsEmpty())
			{
				enter_point = ShaderEnterEnum[i];
			}

			String target = shaderInfo.Target[i];
			if (target.IsEmpty())
			{
				target = DXGetShaderTarget(shader_type, version);
			}

			ID3DBlob* code_buffer = nullptr;
			if (!DX10CompileShader("", content, shaderInfo.ShaderDefines, enter_point, target, &code_buffer))
			{
				return -1;
			}
			this->Reflect(shader_type, code_buffer);
			SAFE_RELEASE(code_buffer);
		}
	}

	return 0;
}
DC_END_NAMESPACE