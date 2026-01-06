 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12ShaderReflect final : public Object
{
	friend class DX11Program;
	DEFAULT_CREATE(DX12ShaderReflect);
	FRIEND_CONSTRUCT_DESTRUCT(DX12ShaderReflect);
	DISALLOW_COPY_ASSIGN(DX12ShaderReflect);
	BEGIN_DERIVED_REFECTION_TYPE(DX12ShaderReflect, Object)
	END_REFECTION_TYPE;

public:
	DX12ShaderReflect() = default;
	~DX12ShaderReflect() {}

	void Reflect(ShaderType type, ID3DBlob* shader_code);
	int  Reflect(const String& path, const ShaderDesc& shaderInfo, const char* version);

public:
	ShaderReflectCBuffers	mCBBuffers;
	ShaderReflectVariables	mCBVariables;
	ShaderReflectTextures	mTextures;
	VertexSemantic			mVertexSemantic = VertexSemantic::None;
};

DC_END_NAMESPACE
