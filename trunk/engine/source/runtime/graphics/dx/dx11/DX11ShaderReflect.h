 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/18
* Description：
*****************************************************************************/
#pragma once

#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX11ShaderReflect final : public Object
{
	friend class DX11Program;
	DEFAULT_CREATE(DX11ShaderReflect);
	FRIEND_CONSTRUCT_DESTRUCT(DX11ShaderReflect);
	DISALLOW_COPY_ASSIGN(DX11ShaderReflect);
	BEGIN_DERIVED_REFECTION_TYPE(DX11ShaderReflect, Object)
	END_REFECTION_TYPE;

public:
	DX11ShaderReflect() = default;
	~DX11ShaderReflect() {}

	void Reflect(ShaderType type, ID3DBlob* shader_code);
	int  Reflect(const String& path, const ShaderDesc& shaderInfo, const char* version);

public:
	ShaderReflectCBuffers	mCBBuffers;
	ShaderReflectVariables	mCBVariables;
	ShaderReflectTextures	mTextures;
	VertexSemantic			mVertexSemantic = VertexSemantic::None;
};

DC_END_NAMESPACE
